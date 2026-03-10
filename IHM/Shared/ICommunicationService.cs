namespace Shared
{
    /// <summary>
    /// Interface permettant de déclencher les évèments de réception de messages et de gérer la connexion à un port série.
    /// </summary>
    public interface ICommunicationService
    {
        // Événement déclenché quand une ligne complète est reçue
        event Action<string> MessageReceived;

        void Connect(string portName);
        void Disconnect();
        void SendCommand(string command);
    }
}
