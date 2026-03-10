namespace VMService
{
    using System;
    using System.Collections.Generic;
    using System.Collections.ObjectModel;
    using System.ComponentModel;
    using System.Runtime.CompilerServices;
    using System.Threading;
    using System.Threading.Tasks;
    using System.Windows.Input;
    using Model;
    using Shared;

    /// <summary>
    /// Service VM principal pour la gestion de l'interface MainWindow.xaml.
    /// </summary>
    public class MainVM : INotifyPropertyChanged
    {
        private readonly ICommunicationService _commService;
        private readonly SynchronizationContext _syncContext;
        private GameModel _gameData;

        public ObservableCollection<string> CommunicationHistory { get; } = new ObservableCollection<string>();

        // Propriétés exposées à la Vue (WPF)
        public int Score => _gameData.Score;
        public int TimeRemaining => _gameData.TimeRemaining;

        public string StatusMessage
        {
            get => _gameData.StatusMessage;
            set
            {
                _gameData.StatusMessage = value;
                OnPropertyChanged();
            }
        }

        // --- Console Port Série ---
        public string[] AvailablePorts => System.IO.Ports.SerialPort.GetPortNames();

        private string _selectedPort;
        public string SelectedPort
        {
            get => _selectedPort;
            set { _selectedPort = value; OnPropertyChanged(); }
        }

        public ICommand ConnectCommand { get; }

        // --- Données de Configuration du Jeu ---
        public List<string> GameModes { get; } = new List<string> {"Reflexe Game", "Memory Game" };

        private string _selectedGameMode;
        public string SelectedGameMode
        {
            get => _selectedGameMode;
            set { _selectedGameMode = value; OnPropertyChanged(); }
        }

        private int _gameDuration = 30; // Temps en secondes
        public int GameDuration
        {
            get => _gameDuration;
            set { _gameDuration = value; OnPropertyChanged(); }
        }

        private bool _isGameRunning;
        public bool IsGameRunning
        {
            get => _isGameRunning;
            set
            {
                _isGameRunning = value;
                OnPropertyChanged();
                OnPropertyChanged(nameof(CanPlay));
            }
        }
        public bool CanPlay => !IsGameRunning;

        public ICommand StartGameCommand { get; }

        public MainVM(ICommunicationService commService)
        {
            // update des données de l'interface graphique sur le bon thread
            _syncContext = SynchronizationContext.Current ?? new SynchronizationContext();

            _gameData = new GameModel();
            _commService = commService;

            SelectedGameMode = GameModes[0];
            StartGameCommand = new RelayCommand(StartGame);
            ConnectCommand = new RelayCommand(ConnectToPort);

            // Abonnement au message reçu
            _commService.MessageReceived += ParseIncomingMessage;
        }

        /// <summary>
        /// Méthode de connextion au port passé en paramètre.
        /// </summary>
        /// <param name="parameter"></param>
        private void ConnectToPort(object parameter)
        {
            if (!string.IsNullOrEmpty(SelectedPort))
            {
                try
                {
                    _commService.Connect(SelectedPort);
                    StatusMessage = $"Connecté à {SelectedPort}";
                }
                catch (Exception ex)
                {
                    StatusMessage = $"Erreur: {ex.Message}";
                }
            }
        }

        /// <summary>
        /// Initialise une partie en envoyant la configuration à la carte STM32.
        /// </summary>
        /// <param name="parameter"></param>
        private async void StartGame(object parameter)
        {
            IsGameRunning = true;

            _gameData.Score = 0;
            _gameData.TimeRemaining = 0;
            CommunicationHistory.Clear();

            // Compte à rebours de 3 secondes
            for (int i = 3; i > 0; i--)
            {
                StatusMessage = $"Départ dans {i}...";
                await Task.Delay(1000);
            }

            string modeTraduit = SelectedGameMode == "Reflexe Game" ? "Reflexe" : "Memoire";

            // Envoie infos à la STM32.
            _commService.SendCommand($"CMD:START:{modeTraduit}:{GameDuration}");
            StatusMessage = "Jeu en cours...";
        }

        /// <summary>
        /// Envoie les données reçues de la carte STM32 à l'interface graphique, en les traitant selon leur type (score, temps, message).
        /// </summary>
        /// <param name="rawMessage"></param>
        private void ParseIncomingMessage(string rawMessage)
        {
            // Exécution threader.
            _syncContext.Post(_ =>
            {
                // Mise à jour de l'historique
                CommunicationHistory.Insert(0, $"{DateTime.Now:HH:mm:ss} - {rawMessage}");

                string[] parts = rawMessage.Split(':');

            if (parts.Length == 2)
            {
                // Mise à jour des données selon la commande
                switch (parts[0])
                {
                    case "SCR":
                        _gameData.Score = int.Parse(parts[1]);
                        OnPropertyChanged(nameof(Score));
                        break;
                    case "TIM":
                        int newTime = int.Parse(parts[1]);
                        _gameData.TimeRemaining = newTime;
                        OnPropertyChanged(nameof(TimeRemaining));

                        if (newTime == 0 && IsGameRunning)
                        {
                            StatusMessage = "Partie terminée ! Appuyez sur BTN_0 (Bleu) pour retourner au menu.";
                        }
                        break;
                    case "MSG":
                        string msg = parts[1];
                        StatusMessage = msg;
                        if (msg.Contains("Menu", StringComparison.OrdinalIgnoreCase))
                        {
                            IsGameRunning = false;
                        }
                        break;
                }
            }
            }, null);
        }

        public event PropertyChangedEventHandler PropertyChanged;
        protected void OnPropertyChanged([CallerMemberName] string name = null) =>
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(name));
    }
}
