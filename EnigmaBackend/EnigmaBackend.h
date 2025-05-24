#pragma once
#include <string>
#include <windows.h>
#include <vcclr.h>
#using <System.dll>
using namespace System;

namespace EnigmaBackend {
	public ref class Enigma
	{
    public:
        // ENCODER
        ref class encoder
        {
        public:
            encoder()
            {
                this->size_alph = 26;
                this->alphabet = gcnew array<uint8_t>{'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};
                
            }
           
            uint8_t encode(uint8_t ch, bool% found)
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
            uint8_t decode(uint8_t code, bool% valid)
            {
                if (code >= this->size_alph)
                {
                    valid = 0;
                    return false;
                }
                valid = true;
                return this->alphabet[code];
            }
        private:
            uint8_t size_alph;
            array<uint8_t>^ alphabet;
        };
        
        // ENIGMA
        ref class enigma
        {
            public:
            enigma()
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
            }
            uint8_t encrypt(uint8_t code, bool% valid)
            {
                uint64_t rotor_queue;
                uint8_t new_code;

                if (code >= this->size_rotor)
                {
                    valid = false;
                    return 0;
                }

                new_code = code;

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
                return new_code;
            }
        private:
            uint64_t counter;
            uint8_t size_rotor;
            uint8_t num_rotors;
            array<uint8_t>^ reflector;
            array<array<uint8_t>^>^ rotors;
            void rotor_shift(uint8_t num)
            {
                char temp = this->rotors[num][this->size_rotor - 1];
                for (int i = this->size_rotor - 1; i > 0; --i)
                {
                    this->rotors[num][i] = this->rotors[num][i - 1];
                }
                this->rotors[num][0] = temp;
            }
            uint8_t rotor_find(uint8_t num, uint8_t code, bool% valid)
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
        };


        System::String^ test_machine(System::String^ input)
        {
            encoder^ enc = gcnew encoder();
            enigma^ enigm = gcnew enigma();

            std::string result{};

            // INPUT CHARS
            uint8_t enc_ch{}, dec_ch{};
            int ch{};
            bool flag{false};
            for (int ch_idx = 0; ch_idx < input->Length; ch_idx++)
            {
                ch = input[ch_idx];

                // ENCODE
                enc_ch = enc->encode(ch, flag);
                if (flag == false)
                {
                    // just print unknown char
                    result.push_back(ch);
                    continue;
                }

                // ENCRYPT/DECRYPT
                enc_ch = enigm->encrypt(enc_ch, flag);
                if (flag == false)
                {
                    fprintf(stderr, "\nencoder put to enigma unknown code (%d)\n", enc_ch);
                    break;
                }

                // DECODE
                dec_ch = enc->decode(enc_ch, flag);
                if (flag == false)
                {
                    fprintf(stderr, "\nenigma put to decoder unknown code (%d)\n", enc_ch);
                    break;
                }

                result.push_back(dec_ch);
            }


            return gcnew System::String(result.c_str());
        }
	};
}
