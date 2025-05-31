using System;
using System.Globalization;
using Avalonia.Data.Converters;

namespace enigma.Converters;

/// <summary>
/// Конвертер замены строчных букв на заглавные.
/// </summary>
public class ToUpperConverter : IValueConverter
{
    /// <inheritdoc/>
    public object? Convert(object? value, Type targetType, object? parameter, CultureInfo culture)
    {
        return value?.ToString()?.ToUpper();
    }

    /// <inheritdoc/>
    public object? ConvertBack(object? value, Type targetType, object? parameter, CultureInfo culture)
    {
        return value?.ToString()?.ToUpper();
    }
}