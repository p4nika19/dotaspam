#include <windows.h>
#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <chrono>

using namespace std;

atomic<bool> f10Pressed = false;
atomic<bool> exitProgram = false;


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

    if (PressKey(VK_RETURN)) return true;

    INPUT shiftUp = { 0 };
    shiftUp.type = INPUT_KEYBOARD;
    shiftUp.ki.wVk = VK_SHIFT;
    shiftUp.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &shiftUp, sizeof(INPUT));

    return f10Pressed;
}


bool TypeString(const string& str) {
    for (char c : str) {
        if (f10Pressed) return true;

        if (c >= '0' && c <= '9') {
            PressKey(0x30 + (c - '0'));
        }
        else if (c == '-') {
            PressKey(VK_OEM_MINUS);
        }
        else if (c == '?') {
            INPUT shiftDown = { 0 };
            shiftDown.type = INPUT_KEYBOARD;
            shiftDown.ki.wVk = VK_SHIFT;
            SendInput(1, &shiftDown, sizeof(INPUT));

            PressKey(VK_OEM_2);

            INPUT shiftUp = { 0 };
            shiftUp.type = INPUT_KEYBOARD;
            shiftUp.ki.wVk = VK_SHIFT;
            shiftUp.ki.dwFlags = KEYEVENTF_KEYUP;
            SendInput(1, &shiftUp, sizeof(INPUT));
        }
        this_thread::sleep_for(chrono::milliseconds(20));
    }
    return f10Pressed;
}


bool SafeSleep(int ms) {
    auto start = chrono::steady_clock::now();
    while (chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start).count() < ms) {
        if (f10Pressed) return true;
        this_thread::sleep_for(chrono::milliseconds(10));
    }
    return false;
}


void KeyListener() {
    while (!exitProgram) {
        if (GetAsyncKeyState(VK_F10) & 0x8000) {
            f10Pressed = true;
            
            while (GetAsyncKeyState(VK_F10) & 0x8000) {
                this_thread::sleep_for(chrono::milliseconds(10));
            }
        }
        this_thread::sleep_for(chrono::milliseconds(10));
    }
}

int main() {
    cout << "ZZZZZZZZZZZ    XXXX     XXXX   CCCCCCCCCCC" << endl;
    cout << "ZZZZZZZZZZZ     XXXX   XXXX    CCCCCCCCCCC" << endl;
    cout << "       ZZZZ      XXXX XXXX     CCCC" << endl;
    cout << "     ZZZZ         XXX XXX      CCCC" << endl;
    cout << "    ZZZZ           XXXXX       CCCC" << endl;
    cout << "   ZZZZ           XXXXXXX      CCCC" << endl;
    cout << " ZZZZ            XXXX XXXX     CCCC" << endl;
    cout << "ZZZZZZZZZZZ     XXXX   XXXX    CCCCCCCCCCC" << endl;
    cout << "ZZZZZZZZZZZ    XXXX     XXXX   CCCCCCCCCCC" << endl;
    cout << endl;
    cout << "dota chat spamer v 0.1a" << endl;
    cout << "made by p4nika";

    int current = 1000;
    bool isRunning = false;

    
    thread listener(KeyListener);
    listener.detach();

    while (!exitProgram) {
        if (f10Pressed) {
            f10Pressed = false;
            isRunning = !isRunning; 
            current = 1000;
            this_thread::sleep_for(chrono::milliseconds(200)); 
        }

        if (isRunning) {
            while (current >= 0) {
                if (f10Pressed) break;

                PressShiftEnter();
                if (SafeSleep(50)) break;

                TypeString(to_string(current) + "-7?");
                if (SafeSleep(50)) break;

                PressKey(VK_RETURN);
                current -= 7;

                if (SafeSleep(50)) break;
            }

            if (current < 0) {
                isRunning = false;
                current = 1000;
            }
        }

        this_thread::sleep_for(chrono::milliseconds(50)); 
    }

    exitProgram = true;
    return 0;
}