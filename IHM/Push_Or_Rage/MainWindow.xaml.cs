using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using VMService;
using Shared;

namespace Push_Or_Rage
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();

            // Initialisation de la communication série (à configurer avec le bon port / mock)
            ICommunicationService commService = new SerialCommunicationService();

            // Connexion du ViewModel à la Vue
            this.DataContext = new MainVM(commService);
        }
    }
}