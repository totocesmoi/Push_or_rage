using System;
using System.Collections.Generic;
using System.Text;
using System.IO.Ports;

namespace Shared
{
    /// <summary>
    /// Interface de méthodes pour établir la communication avec la carte STM32.
    /// </summary>
    public class SerialCommunicationService : ICommunicationService
    {
        
        private SerialPort _serialPort;
        public event Action<string> MessageReceived;

        public SerialCommunicationService()
        {
            _serialPort = new SerialPort { BaudRate = 115200, Parity = Parity.None, DataBits = 8 };
            _serialPort.DataReceived += OnDataReceived;
        }

        /// <summary>
        /// Event trigger lorsque des données sont reçues. Lit la ligne et déclenche l'événement MessageReceived avec les données reçues.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OnDataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            try
            {
                string data = _serialPort.ReadLine();
                MessageReceived?.Invoke(data.Trim());
            }
            catch { }
        }

        /// <summary>
        /// Tentative de connexion sur le port spécifié.
        /// </summary>
        /// <param name="portName"></param>
        public void Connect(string portName)
        {
            if (!_serialPort.IsOpen)
            {
                _serialPort.PortName = portName;
                _serialPort.Open();
            }
        }

        /// <summary>
        /// Déconnexion 
        /// </summary>
        public void Disconnect()
        {
            if (_serialPort.IsOpen)
            {
                _serialPort.Close();
            }
        }

        /// <summary>
        /// Envoie de commande à la carte STM32.
        /// </summary>
        /// <param name="command"></param>
        public void SendCommand(string command)
        {
            if (_serialPort.IsOpen)
            {
                _serialPort.WriteLine(command);
            }
        }
    }
}
