using EnigmaBackend;

namespace enigma.ViewModels
{
    public class MainWindowViewModel : ViewModelBase
    {
#pragma warning disable CA1822 // Mark members as static
        public string Greeting => new EnigmaBackend.Enigma(new EnigmaBackend.EnigmaConfigDto() {
            connections = [
                new connection() { a = 'A', b = 'K' },
                new connection() { a = 'E', b = 'C' },
                new connection() { a = 'X', b = 'L' }
            ],
            rotor_starting_positions = ['A', 'U', 'H']
        }).Encode("OYUWYFHJXNPGIWMOWYKFHGGNPOYUWYFHJXNPGIWMOWYKFHGGNPOO");
#pragma warning restore CA1822 // Mark members as static
    }
}
