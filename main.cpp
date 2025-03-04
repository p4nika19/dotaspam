#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
#include <conio.h>

using namespace std;

atomic<bool> f10Pressed = false;
atomic<bool> exitProgram = false;
atomic<bool> changeSelection = false;
atomic<bool> pauseRequest = false;
atomic<int> selectedText = 0;
vector<wstring> goydaLines;

wstring SanitizeText(const wstring& input) {
    wstring result = input;
    for (wchar_t& c : result) {
        if (c < 32 || c > 0x10FFFF) c = L' ';
    }
    return result;
}

bool PressKey(WORD key) {
    if (f10Pressed) return true;

    INPUT input = { 0 };
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = key;
    SendInput(1, &input, sizeof(INPUT));

    input.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &input, sizeof(INPUT));

    return f10Pressed;
}

bool PressShiftEnter() {
    if (f10Pressed) return true;

    INPUT shiftDown = { 0 };
    shiftDown.type = INPUT_KEYBOARD;
    shiftDown.ki.wVk = VK_SHIFT;
    SendInput(1, &shiftDown, sizeof(INPUT));

    PressKey(VK_RETURN);

    INPUT shiftUp = { 0 };
    shiftUp.type = INPUT_KEYBOARD;
    shiftUp.ki.wVk = VK_SHIFT;
    shiftUp.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &shiftUp, sizeof(INPUT));

    return f10Pressed;
}

bool TypeUnicode(wchar_t ch) {
    if (f10Pressed) return true;

    INPUT inputs[2] = { 0 };
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wScan = ch;
    inputs[0].ki.dwFlags = KEYEVENTF_UNICODE;

    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wScan = ch;
    inputs[1].ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;

    SendInput(2, inputs, sizeof(INPUT));
    return f10Pressed;
}

bool TypeString(const wstring& str) {
    wstring sanitized = SanitizeText(str);
    for (wchar_t c : sanitized) {
        if (f10Pressed) return true;

        if (c == L'\n' || c == L'\r') {
            PressShiftEnter();
        }
        else {
            TypeUnicode(c);
        }
        this_thread::sleep_for(chrono::milliseconds(5));
    }
    return f10Pressed;
}

bool SafeSleep(int ms) {
    auto start = chrono::steady_clock::now();
    while (chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start).count() < ms) {
        if (f10Pressed) return true;
        this_thread::sleep_for(chrono::milliseconds(5));
    }
    return false;
}

void KeyListener() {
    while (!exitProgram) {
        // Обработка Alt+F10
        if ((GetAsyncKeyState(VK_MENU) & 0x8000) && (GetAsyncKeyState(VK_F10) & 0x8000)) {
            changeSelection = true;
            while ((GetAsyncKeyState(VK_MENU) & 0x8000) || (GetAsyncKeyState(VK_F10) & 0x8000)) {
                this_thread::sleep_for(chrono::milliseconds(10));
            }
        }

        // Обработка LShift+F10
        if ((GetAsyncKeyState(VK_LSHIFT) & 0x8000) && (GetAsyncKeyState(VK_F10) & 0x8000)) {
            pauseRequest = true;
            while ((GetAsyncKeyState(VK_LSHIFT) & 0x8000) || (GetAsyncKeyState(VK_F10) & 0x8000)) {
                this_thread::sleep_for(chrono::milliseconds(10));
            }
        }

        // Обработка обычного F10
        if (GetAsyncKeyState(VK_F10) & 0x8000) {
            f10Pressed = true;
            while (GetAsyncKeyState(VK_F10) & 0x8000) {
                this_thread::sleep_for(chrono::milliseconds(10));
            }
        }

        this_thread::sleep_for(chrono::milliseconds(10));
    }
}

void InitGoydaText() {
    goydaLines = {
        L"ГОЙДА  ГОЙДАГОООЙЙЙДАААААААААА!!!!!! ZZZZZVVVVVV",
        L"ZZZZZOOOOOOVVVVVV",
        L"В МЕНЯ ВСЕЛИЛСЯ ДУХ ДРОЧЕСЛАВА БАГИРОВА ИСТИННОГО ПАТРИОТА РОСССИ ZV  СВОИХ НЕ БРОСАЕМ!!!!",
        L"Я РУССКИЙ Я ИДУ ДО КОНЦА Я РУССКИЙ МОЯ КРОВЬ ОТ ОТЦА Я РУССКИЙ И МНЕ ПОВЕЗЛО Я РУССКИЙ ВСЕМУ МИРУ НА ЗЛО",
        L"ШАМАН НАШ БРАТ СЛАВА ПУТИНУ ЧТОБ ОН 1000 ЛЕТ ЖИЛ И КАДЫРОВ БРАТКИ И ЛУКАШЕНКО",
        L"ОНИ ЕДИНСТВЕННЫЕ ЛЮДИ НА ПЛАНЕТЕ У КОГО ЕЩЁ ОСТАЛСЯ РАССУДОК ОНИ ВЫСТУПАЮТ ЗА ВСЕ ХОРОШЕЕ А ЗАПАД ПОКЛОНЯЕТСЯ САТАНЕ ОНИ ФАШИСТЫ",
        L"РОССИЯ ВСЕГДА ПРАВА!!!!!",
        L"ГОООЙДААААААААА  ZOV ",
        L"Какие все тупые только Россия умная",
        L"все сатане поклоняются и они плохие а мы хорошие мы никогда не нападали ни на кого и были за мир",
        L"это нас хотели всех убить мы никому не нравились потому что были добрыми и хорошими а остальные плохими",
        L"с нами бог с русскими так Ванга сказала Путин молодец страна встала с колен ZOV",
        L"ХОХЛЫ СОСАТЬ Я ПРИДУ В КИЕВ НАХУЙ И ЗАСУНУ ЧЕРЕЗ АНУС ЗЕЛЕНСКОМУ ФЛАГ ВЕЛИКОЙ СТРАНЫ",
        L"А ПОТОМ Я ЗАОРУ НА ВСЮ ХОХЛЯНДИЮ: КИЕВ НАШ  Z ПЕРЕМОГИ НЕ БУДЕТ",
        L"А ПОТОМ ВМЕСТЕ С ПОЗДНЯКОВЫМ БУДЕМ ЛОВИТЬ КИЕВЛЯН СО СЛОВАМИ: ТЕПЕРЬ ТЫ РУССКИЙ СОСУНОК Z",
        L"А ПОТОМ КО МНЕ ПРИДЕТ ВЕЛИЧАЙШИЙ ИЗ ВСЕХ БОГОВ ВЛАДИМИР ВЛАДИМИРОВИЧ ПУТИН И ВРУЧИТ МНЕ МЕДАЛЬ И ТАКОЙ: ХОРОШАЯ РАБОТА, СЫНОК Z",
        L"А ПОТОМ Я ПОЙДУ В ПЕНДОСИЮ К БАЙДЕНУ И Я ТАКОЙ: НУ ЧЕ СУКА НЕ ДОИГРАЛСЯ БЛЯТЬ? И Я НАЧНУ ЕГО ЕБАТЬ КРИЧА: Я РУССКИЙ Я ИДУ ДО КОНЦА",
        L"А ПОТОМ КОГДА ЗАКОНЧУ ВСКАЧУ НА СВОЕГО МИШКУ МИЛАХЫЧА",
        L"И ПОСКАЧУ НА НЕМ В ТАЙГУ ПОМОГАТЬ РУССКИМ ЗВЕРЯМ ПОТОМУ ЧТО Я РУССКИЙ"
    };

    for (wstring& line : goydaLines) {
        line = SanitizeText(line);
    }
}

void ShowMenu() {
    system("cls");
    wcout << L"ZZZZZZZZZZZ    XXXX     XXXX   CCCCCCCCCCC" << endl;
    wcout << L"ZZZZZZZZZZZ     XXXX   XXXX    CCCCCCCCCCC" << endl;
    wcout << L"       ZZZZ      XXXX XXXX     CCCC" << endl;
    wcout << L"     ZZZZ         XXX XXX      CCCC" << endl;
    wcout << L"    ZZZZ           XXXXX       CCCC" << endl;
    wcout << L"   ZZZZ           XXXXXXX      CCCC" << endl;
    wcout << L" ZZZZ            XXXX XXXX     CCCC" << endl;
    wcout << L"ZZZZZZZZZZZ     XXXX   XXXX    CCCCCCCCCCC" << endl;
    wcout << L"ZZZZZZZZZZZ    XXXX     XXXX   CCCCCCCCCCC" << endl;
    wcout << endl;
    wcout << L"dota chat spamer v 0.2" << endl;
    wcout << L"made by p4nika" << endl;
    wcout << L"\ncontrols:" << endl;
    wcout << L"alt+f10 - change spam" << endl;
    wcout << L"shift+f10 - pause" << endl;
    wcout << L"f10 - stop" << endl;
    wcout << L"\nselect spam:" << endl;
    wcout << L"1. zxc" << endl;
    wcout << L"2. goida" << endl;
    wcout << L"youe choise (1/2): " << flush;
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    InitGoydaText();
    ShowMenu();

    while (selectedText == 0) {
        if (_kbhit()) {
            int key = _getch();
            if (key == '1') {
                selectedText = 1;
                wcout << L'1' << flush;
            }
            else if (key == '2') {
                selectedText = 2;
                wcout << L'2' << flush;
            }
        }
        this_thread::sleep_for(chrono::milliseconds(10));
    }

    thread listener(KeyListener);
    listener.detach();

    int current = 1000;
    bool isRunning = false;
    size_t goydaIndex = 0;
    bool wasPaused = false;

    while (!exitProgram) {
        if (changeSelection) {
            changeSelection = false;
            isRunning = false;
            selectedText = 0;
            current = 1000;
            goydaIndex = 0;
            ShowMenu();

            while (selectedText == 0) {
                if (_kbhit()) {
                    int key = _getch();
                    if (key == '1') {
                        selectedText = 1;
                        wcout << L'1' << flush;
                    }
                    else if (key == '2') {
                        selectedText = 2;
                        wcout << L'2' << flush;
                    }
                }
                this_thread::sleep_for(chrono::milliseconds(10));
            }
        }

        if (pauseRequest) {
            pauseRequest = false;
            isRunning = false;
            wasPaused = true;
        }

        if (f10Pressed) {
            f10Pressed = false;
            if (!wasPaused) {
                current = 1000;
                goydaIndex = 0;
            }
            isRunning = !isRunning;
            wasPaused = false;
            this_thread::sleep_for(chrono::milliseconds(200));
        }

        if (isRunning) {
            if (selectedText == 1) {
                while (current >= 0 && isRunning) {
                    if (f10Pressed || pauseRequest) break;

                    PressShiftEnter();
                    if (SafeSleep(50)) break;

                    wstring text = to_wstring(current) + L"-7?";
                    TypeString(text);
                    if (SafeSleep(50)) break;

                    PressKey(VK_RETURN);
                    current -= 7;

                    if (SafeSleep(50)) break;
                }
            }
            else if (selectedText == 2) {
                while (goydaIndex < goydaLines.size() && isRunning) {
                    if (f10Pressed || pauseRequest) break;

                    PressShiftEnter();
                    if (SafeSleep(50)) break;

                    TypeString(goydaLines[goydaIndex]);
                    goydaIndex++;

                    PressKey(VK_RETURN);
                    if (SafeSleep(50)) break;
                }
            }

            if (current < 0 || goydaIndex >= goydaLines.size()) {
                isRunning = false;
                current = 1000;
                goydaIndex = 0;
            }
        }

        this_thread::sleep_for(chrono::milliseconds(50));
    }

    exitProgram = true;
    return 0;
}
