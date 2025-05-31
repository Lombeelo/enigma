using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Windows.Documents;
using System.Windows.Forms;
using System.Windows.Input;
using enigma.Models;
using EnigmaBackend;
using ReactiveUI;
using ReactiveUI.Fody.Helpers;

namespace enigma.ViewModels
{
    public class MainWindowViewModel : ViewModelBase
    {
        /// <summary>
        /// Положения роторов (всего 3 ротора).
        /// </summary>
        public List<char> Rotors { get; } = null!;

        /// <summary>
        /// Коммутационная панель.
        /// Максимум 13 пар.
        /// </summary>
        public List<connection> Connections { get; }

        /// <summary>
        /// Текст для шифра.
        /// </summary>
        [Reactive]
        public string InputText { get; set; }

        /// <summary>
        /// Зашифрованный текст.
        /// </summary>
        [Reactive]
        public string OutputText { get; set; }

        /// <summary>
        /// История шифров.
        /// </summary>
        [Reactive]
        public ObservableCollection<HistoryItem> History { get; set; } = new();

        /// <summary>
        /// Энигма.
        /// </summary>
        public Enigma Enigma { get; }

        /// <summary>
        /// Шифровать текст.
        /// </summary>
        public ICommand EncodeCommand { get; }

        /// <summary>
        /// Конструктор.
        /// </summary>
        public MainWindowViewModel()
        {
            Rotors = new()
            {
                'A', 'U', 'H'
            };

            Connections = new()
            {
                new connection() { a = 'A', b = 'K' },
                new connection() { a = 'E', b = 'C' },
                new connection() { a = 'X', b = 'L' }
            };

            Enigma = new Enigma(new EnigmaConfigDto()
            {
                rotor_starting_positions = Rotors.ToArray(),
                connections = Connections.ToArray()
            });

            EncodeCommand = ReactiveCommand.Create(() =>
            {
                if (string.IsNullOrEmpty(InputText)) 
                    return;

                OutputText = Enigma.Encode(InputText);
                History.Add(new HistoryItem(InputText, OutputText));

                InputText = string.Empty;
            });
        }
    }
}
