#pragma once
#include <string>
#include <windows.h>
#include <vcclr.h>
#include <algorithm>
#include <ranges>
#using <System.dll>
using namespace System;

namespace EnigmaBackend {

    // соединение 2-х букв.
    // работает в 2 стороны
    public value struct connection
    {
        Char a, b;
    };

    public value struct EnigmaConfigDto
    {
        // начальные положения роторов. Характеризуются тем, какая буква будет первой в массиве
        array<Char>^ rotor_starting_positions;
        // соединения. ни одна буква не должна повторяться во всём массиве
        array<connection>^ connections;
    };

    // ENIGMA
    ref class enigma_impl
    {
    public:
        enigma_impl();
        enigma_impl(array<Char>^ rotor_starting_positions, array<connection>^ connections);
        uint8_t encrypt(uint8_t code, bool% valid);
    private:
        void rotor_shift(uint8_t num);
        uint8_t rotor_find(uint8_t num, uint8_t code, bool% valid);
        uint8_t pass_through_connection(uint8_t code);

        uint64_t counter{};
        uint8_t size_rotor{};
        uint8_t num_rotors{};
        array<uint8_t>^ reflector;
        array<array<uint8_t>^>^ rotors;
        array<connection>^ connections;
    };

    ref class encoder
    {
    public:
        encoder();
        uint8_t encode(uint8_t ch, bool% found);
        uint8_t decode(uint8_t code, bool% valid);
    private:
        uint8_t size_alph;
        array<uint8_t>^ alphabet;
    };

    // Основной класс для работы из C#
	public ref class Enigma
	{
        encoder^ enc;
        enigma_impl^ enigma;
    public:

        Enigma(EnigmaConfigDto enigma_config) : enc{gcnew encoder()}
        {
            this->construct(enigma_config);
        }

        // тестовая конфигурация - гарантированно рабочая
        Enigma() : enc{ gcnew encoder() }
        {
            this->construct(EnigmaConfigDto{
                    gcnew array<Char> {'A', 'U', 'H'},
                    gcnew array<connection> {{'A', 'K'}, {'E', 'C'}, {'X', 'L'}}
                });
        }

        // Принимает любые знаки, но кодирует только те, что капсом и на латинице
        System::String^ Encode(System::String^ input)
        {
            std::string result{};
            bool flag{false};
            // INPUT CHARS
            uint8_t enc_ch{}, dec_ch{};
            int ch{};
            for (int ch_idx = 0; ch_idx < input->Length; ch_idx++)
            {
                ch = input[ch_idx];

                // ENCODE
                enc_ch = this->enc->encode(ch, flag);
                if (flag == false)
                {
                    // just print unknown char
                    result.push_back(ch);
                    continue;
                }

                // ENCRYPT/DECRYPT
                enc_ch = this->enigma->encrypt(enc_ch, flag);
                if (flag == false)
                {
                    throw gcnew Exception("\nencoder put to enigma_impl unknown code\n");
                    break;
                }

                // DECODE
                dec_ch = this->enc->decode(enc_ch, flag);
                if (flag == false)
                {
                    throw gcnew Exception("\nenigma put to decoder unknown code\n");
                    break;
                }

                result.push_back(dec_ch);
            }

            return gcnew System::String(result.c_str());
        }

        private:
            void construct(EnigmaConfigDto enigma_config)
            {
                // сначала кодируем в символы всю конфигурацию
                bool flag = false;
                for (int i = 0; i < enigma_config.rotor_starting_positions->Length; i++)
                {
                    auto% pos = enigma_config.rotor_starting_positions[i];
                    pos = this->enc->encode(pos, flag);
                    if (flag == false)
                    {
                        throw gcnew Exception("\nunknown rotor position\n");
                    }
                }

                for (int i = 0; i < enigma_config.connections->Length; i++)
                {
                    auto% pos = enigma_config.connections[i];
                    pos.a = this->enc->encode(pos.a, flag);
                    if (flag == false)
                    {
                        throw gcnew Exception("\nunknown rotor connection\n");
                    }
                    pos.b = this->enc->encode(pos.b, flag);
                    if (flag == false)
                    {
                        throw gcnew Exception("\nunknown rotor connection\n");
                    }
                }
                this->enigma = gcnew enigma_impl(enigma_config.rotor_starting_positions, enigma_config.connections);
            }
	};

    // implementation;
    // ENCODER
    encoder::encoder()
    {
        this->size_alph = 26;
        this->alphabet = gcnew array<uint8_t>{'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};

    }

    uint8_t encoder::encode(uint8_t ch, bool% found)
    {
        for (int i = 0; i < this->size_alph; ++i)
        {
            if (this->alphabet[i] == ch)
            {
                found = true;
                return i;
            }
        }
        found = false;
        return 0;
    }
    uint8_t encoder::decode(uint8_t code, bool% valid)
    {
        if (code >= this->size_alph)
        {
            valid = 0;
            return false;
        }
        valid = true;
        return this->alphabet[code];
    }

    // ENIGMA
    
    enigma_impl::enigma_impl()
    {
        // задаём чисто стандартные роторы
        this->size_rotor = 26;
        this->num_rotors = 3;
        this->counter = 0;

        this->reflector = gcnew array<uint8_t>{ 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
        this->rotors = gcnew array<array<uint8_t>^>
        {
            {
                0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25
            },
                {
                    20, 21, 22, 23, 24, 25, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
                },
                {
                    7, 6, 5, 4, 3, 2, 1, 0, 24, 23, 22, 21, 20, 25, 8, 9, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10
                }
        };
        this->connections = gcnew array<connection>(3);

        this->connections[0].a = 0;
        this->connections[0].b = 10;

        this->connections[1].a = 4;
        this->connections[1].b = 2;

        this->connections[2].a = 23;
        this->connections[2].b = 11;
    }

    enigma_impl::enigma_impl(array<Char>^ rotor_starting_positions, array<connection>^ connections) : enigma_impl()
    {
        this->connections = connections;
        // поворачиваем роторы первой буквой к нам
        for (size_t i = 0; i < this->num_rotors; i++)
        {
            while (this->rotors[i][0] != rotor_starting_positions[i])
            {
                this->rotor_shift(i);
            }
        }
    }

    uint8_t enigma_impl::encrypt(uint8_t code, bool% valid)
    {
        uint64_t rotor_queue;
        uint8_t new_code;

        if (code >= this->size_rotor)
        {
            valid = false;
            return 0;
        }

        new_code = this->pass_through_connection(code);

        // code -> rotors
        for (int i = 0; i < this->num_rotors; ++i)
        {
            new_code = this->rotors[i][new_code];
        }

        // code -> reflector
        new_code = this->reflector[new_code];
        // reflector -> code

        // rotors -> code
        for (int i = this->num_rotors - 1; i >= 0; --i)
        {
            new_code = this->rotor_find(i, new_code, valid);
            if (valid == false)
            {
                return 0;
            }
        }



        // shift rotors
        rotor_queue = 1;
        this->counter += 1;
        for (int i = 0; i < this->num_rotors; ++i)
        {
            if (this->counter % rotor_queue == 0)
            {
                this->rotor_shift(i);
            }
            rotor_queue *= this->size_rotor;
        }

        valid = true;
        return  this->pass_through_connection(new_code);
    }

    void enigma_impl::rotor_shift(uint8_t num)
    {
        char temp = this->rotors[num][this->size_rotor - 1];
        for (int i = this->size_rotor - 1; i > 0; --i)
        {
            this->rotors[num][i] = this->rotors[num][i - 1];
        }
        this->rotors[num][0] = temp;
    }
    uint8_t enigma_impl::rotor_find(uint8_t num, uint8_t code, bool% valid)
    {
        for (int i = 0; i < this->size_rotor; ++i)
        {
            if (this->rotors[num][i] == code)
            {
                valid = true;
                return i;
            }
        }
        valid = false;
        return 0;
    }
    uint8_t enigma_impl::pass_through_connection(uint8_t code)
    {
        for (size_t i = 0; i < this->connections->Length; i++)
        {
            auto% item = this->connections[i];
            if (item.a == code)
            {
                return item.b;
            }
            if (item.b == code)
            {
                return item.a;
            }
        }

        return code;
    }

}
