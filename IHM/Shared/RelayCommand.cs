using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Input;

namespace Shared
{
    /// <summary>
    /// Logique de commande pour le pattern MVVM,permettant de lier une action à une commande dans la vue WPF.
    /// </summary>
    public class RelayCommand : ICommand
    {
        private readonly Action<object> _execute;
        public RelayCommand(Action<object> execute) => _execute = execute;
        public event EventHandler CanExecuteChanged { add { } remove { } }
        public bool CanExecute(object parameter) => true;
        public void Execute(object parameter) => _execute(parameter);
    }
}
