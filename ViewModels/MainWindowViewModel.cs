namespace enigma.ViewModels
{
    public class MainWindowViewModel : ViewModelBase
    {
#pragma warning disable CA1822 // Mark members as static
        public string Greeting => new EnigmaBackend.Enigma().test_machine("AAAAAAAAAAAAAAAAAAAAAAAAAAA");
#pragma warning restore CA1822 // Mark members as static
    }
}
