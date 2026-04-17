#include <iostream>
#include<time.h>
#include<windows.h>
#include<cmath>
#include<conio.h>
#include <cstring>
#include<iomanip>
#include<string>
#include<vector>
#include<algorithm>
#include<cctype>//для работы с симолами на примиер isdigit это цыфра ispunct это символы то бишь @#$%^&*
#include <stdexcept> 
#include <fstream>
using namespace std;
enum Color { black, blue, green, cyan, red, magenta, brown, lightgray, darkgray, lightblue, lightgreen, lightcyan, lightred, lightmagenta, yellow, white };
HANDLE hstdout = GetStdHandle(STD_OUTPUT_HANDLE);

void setColor(Color text, Color background)
{
    SetConsoleTextAttribute(hstdout, (WORD)((background << 4) | text));
}
enum Direction { Up = 72, Left = 75, Right = 77, Down = 80, Enter = 13, Esc = 27 };
HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

void setCursor(int x, int y)
{
    COORD myCoords = { x,y };//инициализируем передаваемыми значениями объект координат
    SetConsoleCursorPosition(hStdOut, myCoords);
}


//Карты отрисовывать.На карте есть номинал и масть.В зависимости от масти карты соответствующего цвета - красного или чёрного.
//В левом верхнем углу отрисовывается оставшаяся после раздачи колода.
//На "рубашке" верхней карты отображается количество оставшихся в колоде карт.Рядом показана карта козырной масти.
//Снизу отображаются карты из "руки" пользователя.В верхней часи окна - отображаются карты бота.Карты отображаются "рубашками" вверх.
//Первым осуществляет ход тот, у кого есть козырная карта меньшего номинала.Карту нужно "предъявить" перед первым ходом.
//(Это происходит автоматически и для бота и для пользователя)
//Перемешение между картами пользователя осуществляется стрелочками.
//Выбираемая карты выдвигается вверх из общего ряда.Подтверждение выбора - нажатие Enter.
//Если происходит попытка отбиться - нужно проверять карту на соответствие моменту.Т.е.чтобы выбранной картой можно было отбить карту,
//лежащую на "столе" или подбросить, если отбивается бот.

//Игра осуществляется согласно стандартным правилам лото.
//Игрок выбирает с каким количеством карточек он будет играть.Компьютеру раздают столько же карточек.
//Количество карточек не превышает три.
//Каждая карточка имеет 3 строки и 9 столбцов.
//В каждой строке находится 5 заполненных ячеек и 4 пустые.Позиции ячеек случайные.
//Столбики заполняются значениями следующим образом :
//1 - й столбик - числа от 1 до 9
//2 - й столбик - числа от 10 до 19
//3 - й столбик - числа от 20 до 29
//...
//9 - й столбик - числа от 80 до 90
//В столбике не может быть повторяющихся значений.
//Заполение столбиков реализовать без использования проверки под каждый столбик.
//Карточка отрисовывается с белым фоном и рамкой.
//Навигация по карточке осуществляется стрелочками.Подтверждение выбранной ячейки - нажатием на Enter.
//Числа, которые уже "закрыты" - на карточке отображаются другим цветом.Создавать отдельный массив для хранения выпадавших значений НЕЛЬЗЯ.
//Из "мешка" выпадает случайное число в диапазое от 1 до 90. Числа не повторяются.
//У пльзователя вопрошать : есть ли в его карточке / карточках выпавшее число.Если есть - предлагаетсяуказать ячейку.
//Если пользователь выбирает ячейку, значение в которой не соответствует выпавшему значению - выводится соотвтетствующее моменту сообщение.
//У пользователя три попытки, дабы указать правильную ячейку.
//Карточки компьютера проверяются автоматически.проверять нужно только столбец, в котором МОГЛО бы находится выпавшее число.
//Игра заканчивается после того, как кто - то закрыл все свои карточки.
//В конце игры выводится сообщение о том, кто же выиграл в данный раз.
//После этого выводится меню :
//Новая игра
//Выход

struct user
{
    string login, pass, nick;
};
struct reiting
{
    string nik;
    int ball;
};

void pokaz_reiting();
void pobedi(string imya,int plus_ball);
bool zapis(string& tekushiy_nick);
bool vxod(string& tekushiy_nick);

int menu();
string imya_igroka;
class Loto
{
   
    enum
    {
        maxcount = 91//enum фиксированное число чтобы не писать везде 91
    };
    int data[maxcount];//ячейки где будут лежать 91 (мешок с боченками)
    int count = 0;//счетчик
public:
    Loto()
    {
        pole();
        pere();
    }
    void pole()
    {
        for (int i = 1; i < maxcount; i++)//цикл идет от 1 до 90
        {
            data[count] = i; count++;//кладем боченки в мешок
        }
    }
    void pere()
    {
       
        for (int i = 0; i < count; i++)
        {
           //проходимся по циклу все 90 цифр
            int p = (rand() % count);//создание переменной где будут лежать рандомные числа 
            swap(data[i],data[p]);//в  последовательность меняем на рандом
        }
    }
   int getm()
    {
        if (count == 0)
        {
            throw exception("Пусто! ");//мешок пуст
        }
        int temp = data[0];
        for (int i = 0; i < count-1; i++)
        {
            data[i] = data[i + 1]; // сдвиг вперед удаляет первое число и записывет последующее
        }//так в игре боченок показывает новые числа
          count--;
          return temp;
    }
};
class Card
{
    int map[3][9];
    int closcount = 0;
    int str = 0;
    int st = 0;
public:
    void knopka(int knop)
    {
        if (knop== Up && str > 0)str--;
        if (knop == Down && str < 2)str++;
        if (knop == Left && st > 0)st--;
        if (knop == Right && st < 8)st++;
    }
    void stolb()
    {
       
        for (int j = 0; j < 9; j++) {
            int min = j * 10;
            if (j == 0)
            {
                min = 1; 
            }
            int max = min + 9;
            if (j == 0)
            {
                max = 9;
            }
            if (j == 8)
            {
                max = 90;
            }

            vector<int>v;
            for (int n = min; n <= max; n++)
            {
                v.push_back(n);//копилка мини мешочек
            }


            for (int i = 0; i < 3; i++)
            {
                int idx = (rand() % v.size());
                map[i][j] = v[idx];
                v.erase(v.begin() + idx);
            }
        }
       
    }
    void sort()
    {
        for (int i = 0; i < 3; i++)
        {
            vector<int>pos;
            for (int j = 0; j < 9; j++)
            {
                pos.push_back(j);
            }
            for (int k = 0;  k < 4;  k++)
            {
                int idx = (rand() % pos.size());
                int colnum = pos[idx];
                map[i][colnum] = 0;
                pos.erase(pos.begin() + idx);
            }
        }
    }
    int getval()
    {
        return map[str][st];//покозать число 
   }
    void setclos()
    {
        map[str][st] = -1;//для пометки число закрашенно 
        closcount++;//прибавка единицы к счетчику когда доползет до 15 игра закончится
    }
    bool win()
    {
        return closcount == 15;// если в карте закрыли все ячейки победа
    }
    void pokaz(int x, int y, bool karta) // Добавили координаты x и y
    {
        for (int i = 0; i < 3; i++)
        {
            // Устанавливаем курсор для рисования горизонтальной линии
            setCursor(x, y + (i*2));
            setColor(black, white);
            cout << "=============================================="<<" ";
           
            // Устанавливаем курсор для рисования строки с числами
            setCursor(x, y + 1+ (i * 2));
            cout << "|";

            for (int j = 0; j < 9; j++)
            {
                // Логика цвета зеленый для курсора
                if (karta && i == str && j == st) setColor(black, green);
                else setColor(black, white);

                if (map[i][j] == -1) // Закрыто
                {
                    if (karta && i == str && j == st) setColor(red, green);
                    else setColor(red, white);
                    cout << " XX ";
                }
                else if (map[i][j] == 0) // Пусто
                {
                    cout << "    ";
                }
                else // Число
                {
                    if (karta && i == str && j == st) setColor(black, green);
                    else setColor(black, white);

                    if (map[i][j] < 10) cout << "  " << map[i][j] << " ";
                    else cout << " " << map[i][j] << " ";
                }

                setColor(black, white);
                cout << "|";
            }
            cout << " ";
        }
        // Рисуем самую нижнюю линию
        setCursor(x, y + 6);
        setColor(black, white);
        cout << "=============================================="<<" ";

        setColor(white, black); // Сброс цвета
    }
    void provbot(int prov)
    {
        int stolb = prov / 10;// пример 49/10=4 значит 4 столбец
        //при делении 90/10 будет 9 но у нас 8 столбцов поэтому делаем  иф
        if (prov == 90)
        {
            stolb = 8;
        }
        for (int i = 0; i < 3; i++)
        {
            if (map[i][stolb] == prov)//если у бота совпали числа зачеркиваем
            {
                map[i][stolb] = -1;
                    closcount++; // Добавляем балл в копилку победы (нужно 15 для выигрыша)
            }
        }
    }
};
class Lotgame
{
    Loto meshok;
    vector<Card>mycard;
    vector<Card>botcard;
    int kolcard;
    int activ = 0;
public:
   
    void lotogame(int n)
    {
        kolcard = n;
        for (int i = 0; i < n; i++)
        {
            Card my;
            my.stolb();
            my.sort();
            mycard.push_back(my);
            Card bot;
            bot.stolb();
            bot.sort();
           botcard.push_back(bot);
        }
    }
    bool popal(int b)
    {
        if (mycard[activ].getval() == b)// гетвал курсор = mycard[activ] фактическое место нахождение курсора на карте
        {//сверка если курсор стоит на нужной цифре если они одинаковые
            return true;
        }
        else
        {
            return false;
        }
    }
    void vse_pokaz(int b,int s)
    {
        setCursor(0, 0);
        setColor(white, black);
        cout << "БОЧКА: " << b << " | ШАНСЫ: " << s;
        cout << "   ИГРОК - ";
        setColor(yellow, black);
        cout << "[" << imya_igroka << "]  " << endl;
        setColor(white, black);
        setCursor(13, 2); 
        setColor(green, black);
        cout << "==== ВАШИ КАРТОЧКИ ====";
        setCursor(65, 2); 
        setColor(lightred, black);
        cout << "==== КАРТОЧКИ БОТА ====";
        for (int i = 0; i < kolcard; i++)
        {
            bool focus = (i == activ);//(i == activ) курсор стоит на той карте на которой мы стоим
            mycard[i].pokaz(2, 3 + (i * 8), focus);
        }
       
        for (int i = 0; i < kolcard; i++)
        {
          
            botcard[i].pokaz(55, 3 + (i * 8), false);
        }
        setCursor(0, 27);
        cout <<"Переход по карточкам =  (Tab) "<<"пропустить ход = (пробел) "<<"управление = (стрелками) "<<"для выбора = (ENTER)";
    }
    void play()
    {
        while (true)
        {
            int bochka = meshok.getm();
            int shans = 3;
            setCursor(0, 0);
            while (shans>0)
            {
               
                vse_pokaz(bochka,shans);
                int knop=_getch();
               if (knop == 224)
               {
                   knop = _getch();
               }
               mycard[activ].knopka(knop);
               if (knop == 9)//tab 
               {
                   activ++;// переключатель между картам 
                   if (activ == kolcard)
                   {
                       activ = 0;// так как программа попытается полезть в пустоту тоесть в четвертую которой нет 
                       //сбрасываем тоесть курсор возвращается к первой карточке 
                   }
               }
               if (knop == 32) // Код клавиши Пробел
               {
                   break; // Выход из цикла бочонка если числа нет
               }
               if (knop == 13)
               {
                   if (popal(bochka))
                   {
                       mycard[activ].setclos();
                      /* break;*/
                   }
                   else
                   {
                       shans--;
                   }
               }
            }
            for (int i = 0; i < kolcard; i++)
            {
                botcard[i].provbot(bochka);//бот проверяет каждую свою карту
            }
            for (int i = 0; i < kolcard; i++)
            {
                if (mycard[i].win())
                {
                    pobedi(imya_igroka,kolcard);//kolcard для добавления балла с учетом количества карточек
                    system("cls"); 
                   
                    setColor(white, green); 
                    setCursor(37, 10);
                    cout << "============================" ;
                    setCursor(37, 11);
                    cout << " ПОЗДРАВЛЯЕМ! ВЫ ВЫИГРАЛИ! " ;
                    setCursor(37, 12);
                    cout << "============================" ;
                    _getch();
                    setColor(white,black);
                    return;
                }
               
                 else if (botcard[i].win())
                {
                    system("cls");
                    setColor(white, red); 
                   
                    setCursor(37, 10);
                        cout << "============================" ;
                        setCursor(37, 11);
                        cout << "       УВЫ! БОТ ВЫИГРАЛ!    " ;
                        setCursor(37, 12);
                        cout << "============================" ;
                   
                    _getch();
                    setColor(white, black);
                    return;
                }
            }
        }
    }
    void startend()
    {
        int punkt =0;

        while (true)
        {
            setCursor(35, 10);
            setColor(white, black);
            
            cout << "=== МЕНЮ ИГРЫ ЛОТО ===" << endl;
            setCursor(37, 11);
            if (punkt == 0) cout << "-> "; // Рисуем стрелку если выбран этот пункт
            else            cout << "   "; 
            cout << "Новая игра" << endl;
            setCursor(39, 12);
            //  Выход
            if (punkt == 1) cout << "-> "; // Рисуем стрелку если выбран этот пункт
            else            cout << "   "; 
            cout << "Выход" << endl;

            int k = _getch();
            if (k == 224)
            {
                k = _getch();
                if (k == 72 || k == 80) {
                    if (punkt == 0)
                    {
                        punkt = 1;
                    }
                    else
                    {
                        punkt = 0;
                    }
                }
               
             }
            if (k == 13)
            {
                if (punkt == 1)
                {
                    return;
                }



                if (punkt == 0) // Выбрали НОВУЮ ИГРУ
                {
                    system("cls");
                    setColor(white, black);
                    setCursor(35, 10);
                    int n;
                    cout << "Сколько карточек (1-3)? ";
                   
                    while (!(cin >> n) || n < 1 || n>3)
                    {
                        cin.clear();
                        cin.ignore(1000, '\n');
                        cout << "введите только 1,2 или 3!" << endl;
                   }
                    system("cls");
                    mycard.clear(); // Очищаем векторы от старых игр
                    botcard.clear();

                    meshok = Loto();
                    lotogame(n); // Создаем новые карты
                    play();      // ЗАПУСКАЕМ ПРОЦЕСС бочонки и т.д.

                    // После того как в play кто-то выиграл, возвращаемся в меню
                    system("cls");
                }
            }
        }
  }
};

int main()
{
    setlocale(LC_ALL, "rus");
    srand(time(NULL));

    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

  
    bool dver = false;
    while (!dver)
    {
        int vibor = menu();
        if (vibor == 0)
        {
            if (zapis(imya_igroka))
            {
                dver = true;
            }
        }
        if (vibor == 1)
        {
            if (vxod(imya_igroka))
            {
                dver = true;
            }
        }
        if (vibor == 2)
        {
            system("cls");
            pokaz_reiting();

        }
        if (vibor == 3)
        {
            return 0;
        }

    }
    system("cls");
    Lotgame game;
    game.startend();
}

void pokaz_reiting()
{
    char nazad;
    reiting r;
    ifstream chitay("reiting.txt");

    cout << "=== ТАБЛИЦА ЧЕМПИОНОВ ===" << endl;
    
    if (chitay.is_open())// Проверка если файл нашелся  заходим
    {
        int mesto = 1;
        while (chitay >> r.nik >> r.ball)
        {
            if (mesto == 1)
            {
                setColor(yellow, black);
                cout << mesto << ". ";
               
            }
            if (mesto == 2)
            {
                setColor(cyan, black);
                cout << mesto << ". ";
            }
            if (mesto == 3)
            {
                setColor(red, black);
                cout << mesto << ". ";
            }
            setColor(white, black);
            cout << r.nik << " ---- " << r.ball << " Побед " << endl;
            mesto++;
            Sleep(300);
        }
        chitay.close();
    }
    else
    {
        cout << "Рейтинг пуст!" << endl;
    }
    cout << "нажмите любую клавишу для возврата" << endl;
    nazad = _getch();

}
void pobedi(string imya,int plus_ball)
{
    vector<reiting> v; reiting t;
    bool nashel = false;
    ifstream chit("reiting.txt");
    while (chit >> t.nik >> t.ball)// смотрим на ник и балл
    {
        if (t.nik == imya) //если нашли имя прибавляем балл
        {
            t.ball+=plus_ball;
            nashel = true;
        }
        v.push_back(t);//кидаем в наш вектор имя с баллом
    }
    chit.close();
    if (!nashel)//если игрока не нашли создаем для него данные и кидаем в вектор
    {
        t.nik = imya;
        t.ball = plus_ball;
        v.push_back(t);
    }
    ofstream pishi("reiting.txt");
    for (size_t i = 0; i < v.size(); i++)//берет из нашего вектора данные и записывает их в файл 
    {
        pishi << v[i].nik << "  " << v[i].ball << endl;
    }
    pishi.close();

}
bool zapis(string& tekushiy_nick)
{
    system("cls");
    user noviy, stariy;// Создаем "нового" (кто вводит) и "старого" (кто уже в файле)
    bool zanyato = false;// проверка занято имя или нет
    do
    {

        zanyato = false;
       
        cout << "=== РЕГИСТРАЦИЯ ===" << endl;
        cout << "нажать клавишу 0 и enter (НАЗАД)" << endl;
        cout << "Придумай логин: "; cin >> noviy.login;
        if (noviy.login == "0") return false;
        cout << "Придумай пароль: "; cin >> noviy.pass;
        cout << "Придумай ник: "; cin >> noviy.nick;

        ifstream chtenie("userinfo.txt");
        if (chtenie.is_open())// Если файл вообще существует
        { // Читаем из блокнота парами имя и пароль
            while (chtenie >> stariy.login >> stariy.pass >> stariy.nick)
            { // Сравниваем Ник новичка с Ником из файла (игнорируя размер букв)
                if (_stricmp(noviy.login.c_str(), stariy.login.c_str()) == 0)
                   
                {
                    system("cls");
                    zanyato = true;
                    cout << "ОШИБКА: Логин [" << noviy.login << "] уже занят!" << endl;
                    break;
                }
                if (_stricmp(noviy.nick.c_str(), stariy.nick.c_str()) == 0)
                {
                    zanyato = true;
                    system("cls");
                    cout << "ОШИБКА: Ник [" << noviy.nick << "] уже занят!" << endl;
                    break;
                }
            }
            
            chtenie.close();
        }
       


      /*  if (zanyato)
        {
            system("cls");
            cout << "Ошибка: Такой Ник уже занят!" << endl;

        }*/

    } while (zanyato);

    ofstream zapis("userinfo.txt", ios::app);
    zapis << noviy.login << "  " << noviy.pass << "  " << noviy.nick << endl;
    zapis.close();
    setColor(green, black);
    cout << "Успех! Ты в базе. Нажми любую клавишу..." << endl;
    _getch();
    setColor(white, black);
    tekushiy_nick = noviy.nick;//записываем имя игрока 
    return true;

}
bool vxod(string& tekushiy_nick)
{
    user ya, baza;
    char otmena;
    cout << "Нажми [0] для отмены или любую клавишу для продолжения: ";
    otmena = _getch();
    if (otmena == '0') return false;
    cout << "\nВведите логин: "; cin >> ya.login;

    cout << "Введите пароль: "; cin >> ya.pass;

    ifstream chtenie("userinfo.txt");
    if (chtenie.is_open())
    {
        while (chtenie >> baza.login >> baza.pass >> baza.nick)
        {
            if (baza.login == ya.login && baza.pass == ya.pass)
            {
                tekushiy_nick = baza.nick;
                chtenie.close();
                return true;
            }
        }
    }
    chtenie.close();
    cout << "Ошибка: Неверный логин или пароль!" << endl;
    return false;
}
int menu()
{
    vector<string>punkti = { "РЕГИСТРАЦИЯ", "ВХОД", "РЕЙТИНГ", "ВЫХОД" };
    int activ = 0;
  
    while (true)
    {
        system("cls");
        for (size_t i = 0; i < punkti.size(); i++)
        {
           
            if (i == activ)
            {
                setColor(black, cyan);
            }
            else
            {
                setColor(white, black);

            }
            cout << "  " << punkti[i] << "  " << endl;
        }
        setColor(white, black);
        int key = _getch();
        if (key == 224)
        {
            setColor(white, black);
            key = _getch();
            if (key == Up && activ > 0)activ--;
            if (key == Down && activ < punkti.size() - 1)activ++;
        }
        if (key == 13)
        {
            return activ;
        }
    }
    setColor(white, black);
}


