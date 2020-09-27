#include <iostream>
#include <SDL/SDL.h>
#include <regex>
#include <SDL/SDL_ttf.h>
#include <SDL_draw.h>
#include <map>


using namespace std;

class String {
public:
    virtual string getValue() =0;
    virtual void setValue(string) =0;
};

class StringId: public String{
private:
    string value;
public:
    ~StringId(){};
    StringId(){this->value = "";}
    StringId(string value)
    {
        this->value = value;
    }
    string getValue() override
    {
        return value;
    }
    void setValue(string value) override
    {
        this->value = value;
    }
    int findFirstChar(char target)
    {
        int pos = this->value.find(target);
        if (pos == -1)
        {
            return -1;
        }
        else
        {
            return pos+1;
        }

    }
};

bool operator== (StringId firstStrId, StringId secondStrId)
{
    return firstStrId.getValue() == secondStrId.getValue();
}

class ComplexNumber: public String
{
private:
    int img, real;
public:
    ~ComplexNumber(){}
    ComplexNumber()
    {
        this->img = 0.0;
        this->real = 0.0;
    }
    ComplexNumber(string inputStr)
    {
        pair<int, int> input = transformStr(inputStr);
        this->real = input.first;
        this->img = input.second;
    }
    ComplexNumber(int real, int img)
    {
        this->real = real;
        this->img = img;
    }
    void setValue(string inputStr) override
    {
        pair<int, int> input = transformStr(inputStr);
        this->real = input.first;
        this->img = input.second;
    }
    string getValue() override
    {
        return string(to_string(this->real) + "i" + to_string(this->img));
    }
    static pair<int, int> transformStr(string inputStr)
    {
        int real, img;
        regex regex(R"(((\+|-)?[0-9]*)?i((\+|-)?[0-9]*)?)");
        cmatch match;
        regex_match(inputStr.c_str(), match, regex);
        if (match.size() == 0)
        {
            img = 0;
            real = 0;
        }
        else
        {
            real = stoi(match.str(1));
            //if (match.str(2) == "-") { real *= -1;}
            img = stoi(match.str(3));
            //if (match.str(4) == "-") { img *= -1;}
        }
        return pair<int, int>(real, img);
    }
};

bool operator== (ComplexNumber &firstCN, ComplexNumber &secondCN)
{

    return firstCN.getValue() == secondCN.getValue();
}

ComplexNumber* operator* (ComplexNumber firstCN,  ComplexNumber secondCN)
{
    pair <int, int> transFirst, transSecond, result;
    transFirst = ComplexNumber::transformStr(firstCN.getValue());
    transSecond = ComplexNumber::transformStr(secondCN.getValue());
    result.first = transFirst.first*transSecond.first - transFirst.second*transSecond.second;
    result.second = transFirst.first * transSecond.second + transFirst.second * transSecond.first;
    return new ComplexNumber(result.first, result.second);
}
void printMenu(SDL_Surface *targetSurface, vector <pair<string, int>> &currentMenu,
               SDL_Color &color)
{
    SDL_Surface *textSurf = NULL;

    SDL_Rect dest;
    dest.x = 5;
    dest.y = 5;
    dest.w = 1;
    dest.h = 1;
    for (auto el : currentMenu)
    {
        TTF_Font* font = TTF_OpenFont("font.ttf", el.second);
        textSurf = TTF_RenderUTF8_Solid(font, el.first.c_str(), color);
        SDL_BlitSurface(textSurf, NULL, targetSurface, &dest);
        dest.y += el.second + 5;
        SDL_FreeSurface(textSurf);
        textSurf = NULL;
        TTF_CloseFont(font);
    }
}
int main()
{
    bool workingState = true;
    SDL_Surface *screen;
    if (SDL_Init(SDL_INIT_VIDEO) < 0 )
    {
        workingState = false;
    }
    screen = SDL_SetVideoMode(640, 480, 8, SDL_ANYFORMAT);
    if (screen == NULL)
    {
        workingState = false;
    }

    // хендлер ивентов
    SDL_Event event;

    TTF_Init();


    SDL_Color color;
    color.b = 80;
    color.g = 255;
    color.r = 0;
    // вектора с созданными объектами
    vector <ComplexNumber> dataComplex;
    vector <StringId> dataId;

    // менюшки

    char currentMenu = 'A';
    vector <pair<string, int>> main =
    {
        pair<string, int>("Добро пожаловать!", 36),
        pair<string, int>("Выберете раздел", 24),
        pair<string, int>("1. Комплексное число", 24),
        pair<string, int>("2. Строковый идентификатор", 24),
        pair<string, int>("**************************", 24),
        pair<string, int>("Для выхода - закройте это окно", 24)
    }, complex =
    {
        pair<string, int>("Раздел - Комплексные числа", 36),
        pair<string, int>("Выберете действие", 24),
        pair<string, int>("1. Посмотреть записанные числа", 24),
        pair<string, int>("2. Записать новое число", 24),
        pair<string, int>("3. Перемножить два числа", 24),
        pair<string, int>("4. Сравнить два числа", 24),
        pair<string, int>("Нажмите ESC чтобы вернуться", 24)
    }, id
    {
        pair<string, int>("Раздел - Строковый идентификатор", 36),
        pair<string, int>("Выберете действие", 24),
        pair<string, int>("1. Посмотреть записанные идентификаторы", 24),
        pair<string, int>("2. Записать новый идентификатор", 24),
        pair<string, int>("3. Найти первое вхождение символа", 24),
        pair<string, int>("4. Сравнить два идентификатора", 24),
        pair<string, int>("Нажмите ESC чтобы вернуться", 24)
    }, listComplex, addComplex, multComplex, compareComplex,
            addId, listId, findId, compareId;
    printMenu(screen, main, color);

    // не придумал как нормально трансформировать код кнопки с ивента
    // так что как-то так
    map <int, char> transformKey = {
        pair <int, char> (10, '1'),
        pair <int, char> (11, '2'),
        pair <int, char> (12, '3'),
        pair <int, char> (13, '4'),
        pair <int, char> (14, '5'),
        pair <int, char> (15, '6'),
        pair <int, char> (16, '7'),
        pair <int, char> (17, '8'),
        pair <int, char> (18, '9'),
        pair <int, char> (19, '0'),
        pair <int, char> (38, 'a'),
        pair <int, char> (56, 'b'),
        pair <int, char> (54, 'c'),
        pair <int, char> (40, 'd'),
        pair <int, char> (26, 'e'),
        pair <int, char> (41, 'f'),
        pair <int, char> (42, 'g'),
        pair <int, char> (43, 'h'),
        pair <int, char> (31, 'i'),
        pair <int, char> (44, 'j'),
        pair <int, char> (45, 'k'),
        pair <int, char> (46, 'l'),
        pair <int, char> (58, 'm'),
        pair <int, char> (57, 'n'),
        pair <int, char> (32, 'o'),
        pair <int, char> (33, 'p'),
        pair <int, char> (24, 'q'),
        pair <int, char> (27, 'r'),
        pair <int, char> (39, 's'),
        pair <int, char> (28, 't'),
        pair <int, char> (30, 'u'),
        pair <int, char> (55, 'v'),
        pair <int, char> (25, 'w'),
        pair <int, char> (53, 'x'),
        pair <int, char> (29, 'y'),
        pair <int, char> (52, 'z'),
        pair <int, char> (20, '-'),
        pair <int, char> (21, '+')
    };
    string input = "";
    // для проверки какое число вводится
    bool mult = false;
    string mult1 = "", mult2 = "";

    while (workingState)
    {

        if (SDL_PollEvent(&event) == 0)
        {
            // ОСТАТКИ ЭКСПЕРИМЕНТА С РИСОВАНИЕМ И ТОЧКАМИ
            /*
            Uint32 yellow;
            yellow = SDL_MapRGB(screen->format, 0xff, 0xff, 0x00);
            if ( SDL_MUSTLOCK(screen) ) {
                if ( SDL_LockSurface(screen) < 0 ) {
                    fprintf(stderr, "Can't lock screen: %s\n", SDL_GetError());
                    break;
                }
            }


            //Draw_Line(screen,300, 300, 400, 400,0xFF0000);

            if ( SDL_MUSTLOCK(screen) ) {
                SDL_UnlockSurface(screen);
            }
            // Update just the part of the display that we've changed
            */
            SDL_UpdateRect(screen, 0, 0, 0, 0);

        }
        else
        {
            switch (event.type)
            {
                case SDL_QUIT:
                {
                    workingState = false;
                    break;
                }
                case SDL_KEYDOWN:
                {
                    char keyCode = event.key.keysym.scancode;
                    //cout << "hey " << (int) keyCode << endl;
                    /* пояснение по кейсам
                     * A - главное меню
                     * B - подменю для комплексных чисел
                     * C - подменю для идентификаторов
                     * D - функция списка комплексных чисел
                     * E - функция записи нового комплексного числа
                     * F - функция умножения комплексных чисел
                     * G - функция сравнения комплексных чисел
                     * H - функция списка идентификаторов
                     * I - функция записи нового идентификатора
                     * J - функция поиска первого вхождения символа в идентификатор
                     * K - функция сравнения идентификаторов
                     */
                    switch(currentMenu)
                    {
                        case 'A':
                        {
                            if ((int) keyCode == 10)
                            {
                                currentMenu = 'B';
                                SDL_Delay(100);
                                SDL_FillRect(screen, NULL, 0x000000);
                                printMenu(screen, complex, color);
                            }
                            if ((int) keyCode == 11)
                            {
                                currentMenu = 'C';
                                SDL_Delay(100);
                                SDL_FillRect(screen, NULL, 0x000000);
                                printMenu(screen, id, color);
                            }
                            break;
                        }
                        case 'B':
                        {
                            if ((int) keyCode == 9)
                            {
                                currentMenu = 'A';
                                SDL_Delay(100);
                                SDL_FillRect(screen, NULL, 0x000000);
                                printMenu(screen, main, color);
                            }
                            if ((int) keyCode == 10)
                            {
                                currentMenu = 'D';

                                if (dataComplex.size() == 0)
                                {
                                    listComplex.push_back(pair<string, int> ("Список записанных чисел:", 36));
                                    listComplex.push_back(pair<string, int> ("Не записано ни одного числа ((", 24));
                                    listComplex.push_back(pair<string, int> ("Нажмите ESC чтобы вернуться", 24));
                                }
                                else
                                {
                                    listComplex.push_back(pair<string, int> ("Список записанных чисел:", 36));
                                    for(size_t i = 0; i < dataComplex.size(); i++)
                                    {
                                        listComplex.push_back(pair<string, int> (to_string(i+ 1) + ". " + dataComplex[i].getValue(), 24));
                                    }
                                    listComplex.push_back(pair<string, int> ("Нажмите ESC чтобы вернуться", 24));
                                }

                                SDL_Delay(100);
                                SDL_FillRect(screen, NULL, 0x000000);
                                printMenu(screen, listComplex, color);
                            }
                            if ((int) keyCode == 11)
                            {
                                currentMenu = 'E';
                                addComplex.push_back(pair<string, int> ("Введите комплексное число:", 36));
                                addComplex.push_back(pair<string, int> (input, 24));
                                addComplex.push_back(pair<string, int> ("Пример корректного формата:", 24));
                                addComplex.push_back(pair<string, int> ("33i+12, -7i100, +5i-21", 24));
                                addComplex.push_back(pair<string, int> ("Нажмите Enter чтобы закончить ввод", 24));
                                addComplex.push_back(pair<string, int> ("Нажмите ESC чтобы вернуться", 24));
                                SDL_Delay(100);
                                SDL_FillRect(screen, NULL, 0x000000);
                                printMenu(screen, addComplex, color);
                            }
                            if ((int) keyCode == 12)
                            {
                                currentMenu = 'F';
                                multComplex.push_back(pair<string, int> ("Введите два комплексных числа:", 36));
                                multComplex.push_back(pair<string, int> ("", 24));
                                multComplex.push_back(pair<string, int> ("", 24));
                                multComplex.push_back(pair<string, int> ("", 24));
                                multComplex.push_back(pair<string, int> ("Нажмите Enter чтобы закончить ввод числа", 24));
                                multComplex.push_back(pair<string, int> ("Нажмите ESC чтобы вернуться", 24));
                                SDL_Delay(100);
                                SDL_FillRect(screen, NULL, 0x000000);
                                printMenu(screen, multComplex, color);
                            }
                            if ((int) keyCode == 13)
                            {
                                currentMenu = 'G';
                                compareComplex.push_back(pair<string, int> ("Введите два комплексных числа:", 36));
                                compareComplex.push_back(pair<string, int> ("", 24));
                                compareComplex.push_back(pair<string, int> ("", 24));
                                compareComplex.push_back(pair<string, int> ("", 24));
                                compareComplex.push_back(pair<string, int> ("Нажмите Enter чтобы закончить ввод числа", 24));
                                compareComplex.push_back(pair<string, int> ("Нажмите ESC чтобы вернуться", 24));
                                SDL_Delay(100);
                                SDL_FillRect(screen, NULL, 0x000000);
                                printMenu(screen, compareComplex, color);
                            }
                            break;
                        }
                        case 'D':
                        {
                            if ((int) keyCode == 9)
                            {
                                currentMenu = 'B';
                                listComplex.clear();
                                SDL_Delay(100);
                                SDL_FillRect(screen, NULL, 0x000000);
                                printMenu(screen, complex, color);
                            }
                            break;
                        }
                        case 'E':
                        {
                            if ((int) keyCode == 9)
                            {
                                input = "";
                                addComplex.clear();
                                currentMenu = 'B';
                                SDL_Delay(100);
                                SDL_FillRect(screen, NULL, 0x000000);
                                printMenu(screen, complex, color);
                            }
                            if (((int) keyCode >= 10 && (int) keyCode <=21) || (int) keyCode == 31)
                            {
                                input += transformKey.find((int) keyCode)->second;
                                addComplex[1] = pair<string, int> (input, 24);
                                SDL_FillRect(screen, NULL, 0x000000);
                                printMenu(screen, addComplex, color);
                            }
                            if ((int) keyCode == 36)
                            {
                                dataComplex.push_back(ComplexNumber(input));
                                input = "";
                                addComplex.clear();
                                currentMenu = 'B';
                                SDL_Delay(100);
                                SDL_FillRect(screen, NULL, 0x000000);
                                printMenu(screen, complex, color);
                            }
                            if ((int) keyCode == 22)
                            {
                                if (input.length() > 0)
                                {
                                    input.pop_back();
                                    addComplex[1] = pair<string, int> (input, 24);
                                    SDL_Delay(100);
                                    SDL_FillRect(screen, NULL, 0x000000);
                                    printMenu(screen, addComplex, color);
                                }
                            }
                            break;
                        }
                        case 'F':
                        {
                            if ((int) keyCode == 9)
                            {
                                mult1 = "";
                                mult2 = "";
                                mult = false;
                                multComplex.clear();
                                currentMenu = 'B';
                                SDL_Delay(100);
                                SDL_FillRect(screen, NULL, 0x000000);
                                printMenu(screen, complex, color);
                            }
                            if (((int) keyCode >= 10 && (int) keyCode <=21) || (int) keyCode == 31)
                            {
                                if (!mult)
                                {
                                    mult1 += transformKey.find((int) keyCode)->second;
                                    multComplex[1] = pair<string, int> (mult1, 24);
                                }
                                else
                                {
                                    mult2 += transformKey.find((int) keyCode)->second;
                                    multComplex[2] = pair<string, int> (mult2, 24);
                                }
                                SDL_FillRect(screen, NULL, 0x000000);
                                printMenu(screen, multComplex, color);
                            }
                            if ((int) keyCode == 36)
                            {
                                if (!mult)
                                {
                                    mult = true;
                                }
                                else
                                {
                                    ComplexNumber first = ComplexNumber(mult1), second = ComplexNumber(mult2);
                                    string res = (first * second)->getValue();
                                    multComplex[3] = pair<string, int> ("Результат равен " + res, 24);
                                    SDL_Delay(100);
                                    SDL_FillRect(screen, NULL, 0x000000);
                                    printMenu(screen, multComplex, color);
                                }
                            }
                            if ((int) keyCode == 22)
                            {
                                if (!mult)
                                {
                                    if (mult1.length() > 0)
                                    {
                                        mult1.pop_back();
                                        multComplex[1] = pair<string, int> (mult1, 24);
                                    }
                                }
                                else
                                {
                                    if (mult2.length() > 0)
                                    {
                                        mult2.pop_back();
                                        multComplex[2] = pair<string, int> (mult2, 24);
                                    }
                                }
                                SDL_Delay(100);
                                SDL_FillRect(screen, NULL, 0x000000);
                                printMenu(screen, multComplex, color);

                            }
                            break;
                        }
                        case 'G':
                        {
                            if ((int) keyCode == 9)
                            {
                                mult1 = "";
                                mult2 = "";
                                mult = false;
                                compareComplex.clear();
                                currentMenu = 'B';
                                SDL_Delay(100);
                                SDL_FillRect(screen, NULL, 0x000000);
                                printMenu(screen, complex, color);
                            }
                            if (((int) keyCode >= 10 && (int) keyCode <=19) || (int) keyCode == 31 )
                            {
                                if (!mult)
                                {
                                    mult1 += transformKey.find((int) keyCode)->second;
                                    compareComplex[1] = pair<string, int> (mult1, 24);
                                }
                                else
                                {
                                    mult2 += transformKey.find((int) keyCode)->second;
                                    compareComplex[2] = pair<string, int> (mult2, 24);
                                }
                                SDL_FillRect(screen, NULL, 0x000000);
                                printMenu(screen, compareComplex, color);
                            }
                            if ((int) keyCode == 36)
                            {
                                if (!mult)
                                {
                                    mult = true;
                                }
                                else
                                {
                                    ComplexNumber first = ComplexNumber(mult1), second = ComplexNumber(mult2);
                                    if (first == second)
                                    {
                                        compareComplex[3] = pair<string, int> ("Результат - числа равны", 24);
                                    }
                                    else
                                    {
                                        compareComplex[3] = pair<string, int> ("Результат - числа не равны", 24);
                                    }
                                    SDL_Delay(100);
                                    SDL_FillRect(screen, NULL, 0x000000);
                                    printMenu(screen, compareComplex, color);
                                }
                            }
                            if ((int) keyCode == 22)
                            {
                                if (!mult)
                                {
                                    if (mult1.length() > 0)
                                    {
                                        mult1.pop_back();
                                        compareComplex[1] = pair<string, int> (mult1, 24);
                                    }
                                }
                                else
                                {
                                    if (mult2.length() > 0)
                                    {
                                        mult2.pop_back();
                                        compareComplex[2] = pair<string, int> (mult2, 24);
                                    }
                                }
                                SDL_Delay(100);
                                SDL_FillRect(screen, NULL, 0x000000);
                                printMenu(screen, compareComplex, color);

                            }
                            break;
                        }
                        case 'C':
                        {
                            if ((int) keyCode == 9)
                            {
                                currentMenu = 'A';
                                SDL_Delay(100);
                                SDL_FillRect(screen, NULL, 0x000000);
                                printMenu(screen, main, color);
                            }
                            if ((int) keyCode == 10)
                            {
                                currentMenu = 'H';

                                if (dataId.size() == 0)
                                {
                                    listId.push_back(pair<string, int> ("Список записанных индексов:", 36));
                                    listId.push_back(pair<string, int> ("Не записано ни одного идентификатора ((", 24));
                                    listId.push_back(pair<string, int> ("Нажмите ESC чтобы вернуться", 24));
                                }
                                else
                                {
                                    listId.push_back(pair<string, int> ("Список записанных индексов:", 36));
                                    for(size_t i = 0; i < dataId.size(); i++)
                                    {
                                        listId.push_back(pair<string, int> (to_string(i+ 1) + ". " + dataId[i].getValue(), 24));
                                    }
                                    listId.push_back(pair<string, int> ("Нажмите ESC чтобы вернуться", 24));
                                }

                                SDL_Delay(100);
                                SDL_FillRect(screen, NULL, 0x000000);
                                printMenu(screen, listId, color);
                            }
                            if ((int) keyCode == 11)
                            {
                                currentMenu = 'I';
                                addId.push_back(pair<string, int> ("Введите новый идентификатор:", 36));
                                addId.push_back(pair<string, int> (input, 24));
                                addId.push_back(pair<string, int> ("Нажмите Enter чтобы закончить ввод", 24));
                                addId.push_back(pair<string, int> ("Нажмите ESC чтобы вернуться", 24));
                                SDL_Delay(100);
                                SDL_FillRect(screen, NULL, 0x000000);
                                printMenu(screen, addId, color);
                            }
                            if ((int) keyCode == 12)
                            {
                                currentMenu = 'J';
                                findId.push_back(pair<string, int> ("Введите идентификатор и символ:", 36));
                                findId.push_back(pair<string, int> ("", 24));
                                findId.push_back(pair<string, int> ("", 24));
                                findId.push_back(pair<string, int> ("", 24));
                                findId.push_back(pair<string, int> ("Нажмите Enter чтобы закончить ввод параметра", 24));
                                findId.push_back(pair<string, int> ("Нажмите ESC чтобы вернуться", 24));
                                SDL_Delay(100);
                                SDL_FillRect(screen, NULL, 0x000000);
                                printMenu(screen, findId, color);
                            }
                            if ((int) keyCode == 13)
                            {
                                currentMenu = 'K';
                                compareId.push_back(pair<string, int> ("Введите два идентификатора:", 36));
                                compareId.push_back(pair<string, int> ("", 24));
                                compareId.push_back(pair<string, int> ("", 24));
                                compareId.push_back(pair<string, int> ("", 24));
                                compareId.push_back(pair<string, int> ("Нажмите Enter чтобы закончить ввод идентификатора", 24));
                                compareId.push_back(pair<string, int> ("Нажмите ESC чтобы вернуться", 24));
                                SDL_Delay(100);
                                SDL_FillRect(screen, NULL, 0x000000);
                                printMenu(screen, compareId, color);
                            }

                            break;
                        }
                        case 'H':
                        {
                            if ((int) keyCode == 9)
                            {
                                currentMenu = 'C';
                                listId.clear();
                                SDL_Delay(100);
                                SDL_FillRect(screen, NULL, 0x000000);
                                printMenu(screen, id, color);
                            }
                            break;
                        }
                        case 'I':
                        {
                            if ((int) keyCode == 9)
                            {
                                currentMenu = 'C';
                                addId.clear();
                                SDL_Delay(100);
                                SDL_FillRect(screen, NULL, 0x000000);
                                printMenu(screen, id, color);
                            }
                            if (((int) keyCode >= 10 && (int) keyCode <=19) ||
                                ((int) keyCode >= 24 && (int) keyCode <= 33) ||
                                ((int) keyCode >= 38 && (int) keyCode <= 46) ||
                                ((int) keyCode >= 52 && (int) keyCode <= 58))

                            {
                                input += transformKey.find((int) keyCode)->second;
                                addId[1] = pair<string, int> (input, 24);
                                SDL_FillRect(screen, NULL, 0x000000);
                                printMenu(screen, addId, color);
                            }
                            if ((int) keyCode == 36)
                            {
                                dataId.push_back(StringId(input));
                                input = "";
                                addId.clear();
                                currentMenu = 'C';
                                SDL_Delay(100);
                                SDL_FillRect(screen, NULL, 0x000000);
                                printMenu(screen, id, color);
                            }
                            if ((int) keyCode == 22)
                            {
                                if (input.length() > 0)
                                {
                                    input.pop_back();
                                    addId[1] = pair<string, int> (input, 24);
                                    SDL_Delay(100);
                                    SDL_FillRect(screen, NULL, 0x000000);
                                    printMenu(screen, addId, color);
                                }
                            }
                            break;
                        }
                        case 'J':
                        {
                            if ((int) keyCode == 9)
                            {
                                mult = false;
                                mult1 = "";
                                mult2 = "";
                                currentMenu = 'C';
                                SDL_Delay(100);
                                findId.clear();
                                SDL_FillRect(screen, NULL, 0x000000);
                                printMenu(screen, id, color);
                            }
                            if (((int) keyCode >= 10 && (int) keyCode <= 19) ||
                                ((int) keyCode >= 24 && (int) keyCode <= 33) ||
                                ((int) keyCode >= 38 && (int) keyCode <= 46) ||
                                ((int) keyCode >= 52 && (int) keyCode <= 58))
                            {
                                if (!mult)
                                {
                                    mult1 += transformKey.find((int) keyCode)->second;
                                    findId[1] = pair<string, int> (mult1, 24);
                                }
                                else
                                {
                                    mult2 = transformKey.find((int) keyCode)->second;
                                    findId[2] = pair<string, int> (mult2, 24);
                                }
                                SDL_FillRect(screen, NULL, 0x000000);
                                printMenu(screen, findId, color);
                            }
                            if ((int) keyCode == 36)
                            {
                                if (!mult)
                                {
                                    mult = true;
                                }
                                else
                                {
                                    StringId first = StringId(mult1);
                                    int res = first.findFirstChar(mult2[0]);
                                    if (res != -1)
                                    {
                                        findId[3] = pair<string, int> ("Первое вхождение '" + mult2 + "' - " + to_string(res), 24);
                                    }
                                    else
                                    {
                                        findId[3] = pair<string, int> ("Символа '" + mult2 + "' нету в идентифекаторе", 24);
                                    }
                                    SDL_Delay(100);
                                    SDL_FillRect(screen, NULL, 0x000000);
                                    printMenu(screen, findId, color);
                                }
                            }
                            if ((int) keyCode == 22)
                            {
                                if (!mult)
                                {
                                    if (mult1.length() > 0)
                                    {
                                        mult1.pop_back();
                                        findId[1] = pair<string, int> (mult1, 24);
                                    }
                                }
                                else
                                {
                                    if (mult2.length() > 0)
                                    {
                                        mult2.pop_back();
                                        findId[2] = pair<string, int> (mult2, 24);
                                    }
                                }
                                SDL_Delay(100);
                                SDL_FillRect(screen, NULL, 0x000000);
                                printMenu(screen, findId, color);

                            }
                            break;
                        }
                        case 'K':
                        {
                            if ((int) keyCode == 9)
                            {
                                currentMenu = 'C';
                                mult = false;
                                mult1 = "";
                                mult2 = "";
                                compareId.clear();
                                SDL_Delay(100);
                                SDL_FillRect(screen, NULL, 0x000000);
                                printMenu(screen, id, color);
                            }
                            if (((int) keyCode >= 10 && (int) keyCode <= 19) ||
                                ((int) keyCode >= 24 && (int) keyCode <= 33) ||
                                ((int) keyCode >= 38 && (int) keyCode <= 46) ||
                                ((int) keyCode >= 52 && (int) keyCode <= 58))
                            {
                                if (!mult)
                                {
                                    mult1 += transformKey.find((int) keyCode)->second;
                                    compareId[1] = pair<string, int> (mult1, 24);
                                }
                                else
                                {
                                    mult2 += transformKey.find((int) keyCode)->second;
                                    compareId[2] = pair<string, int> (mult2, 24);
                                }
                                SDL_FillRect(screen, NULL, 0x000000);
                                printMenu(screen, compareId, color);
                            }
                            if ((int) keyCode == 36)
                            {
                                if (!mult)
                                {
                                    mult = true;
                                }
                                else
                                {
                                    StringId first = StringId(mult1), second = StringId(mult2);
                                    if (first == second)
                                    {
                                        compareId[3] = pair<string, int> ("Идентификаторы идентичны", 24);
                                    }
                                    else
                                    {
                                        compareId[3] = pair<string, int> ("Идентификаторы не идентичны", 24);
                                    }
                                    SDL_Delay(100);
                                    SDL_FillRect(screen, NULL, 0x000000);
                                    printMenu(screen, compareId, color);
                                }
                            }
                            if ((int) keyCode == 22)
                            {
                                if (!mult)
                                {
                                    if (mult1.length() > 0)
                                    {
                                        mult1.pop_back();
                                        compareId[1] = pair<string, int> (mult1, 24);
                                    }
                                }
                                else
                                {
                                    if (mult2.length() > 0)
                                    {
                                        mult2.pop_back();
                                        compareId[2] = pair<string, int> (mult2, 24);
                                    }
                                }
                                SDL_Delay(100);
                                SDL_FillRect(screen, NULL, 0x000000);
                                printMenu(screen, compareId, color);

                            }
                            break;
                        }
                    }
                    break;
                }
            }
        }
    }
    TTF_Quit();
    SDL_Quit();

    return 0;
}



