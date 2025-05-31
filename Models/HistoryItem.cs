namespace enigma.Models
{
    /// <summary>
    /// Объект истории шифрования.
    /// </summary>
    public class HistoryItem
    {
        /// <summary>
        /// Введенный текст.
        /// </summary>
        public string InputHistoryText { get; set; }
        
        /// <summary>
        /// Зашифрованный текст.
        /// </summary>
        public string OutputHistoryText { get; set; }

        /// <summary>
        /// Конструктор.
        /// </summary>
        public HistoryItem(string inputHistoryText, string outputHistoryText)
        {
            InputHistoryText = inputHistoryText;
            OutputHistoryText = outputHistoryText;
        }
    }
}
