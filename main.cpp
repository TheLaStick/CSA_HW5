#include <iostream>
#include <thread>
#include <mutex>
#include <random>

struct Room {
    int capacity_;

    /// Четыре вида статуса:
    /// 1) 3 - Полная комната
    /// 2) 2 - В комнате джентельмен
    /// 3) 1 - В комнате дама
    /// 4) 0 - Пустая комната
    int status_;

    Room() {
        status_ = 0;
        capacity_ = 0;
    };

    explicit Room(int capacity) {
        this->capacity_ = capacity;
        status_ = 0;
    }
};

const int kOneCapacityRoomsCount = 10;
const int kTwoCapacityRoomsCount = 15;

const int rooms_count = kOneCapacityRoomsCount + kTwoCapacityRoomsCount;

Room rooms[rooms_count];
std::mutex rooms_mutex;


/// Описывает поведение джентельмена
void gentlemanMethod() {
    for (auto &room: rooms) {
        rooms_mutex.lock();

        if (room.status_ == 0) {
            if (room.capacity_ == 1) {
                room.status_ = 3;
                std::cout << "gentleman enters the empty room with 1 seat" << '\n';
            } else if (room.capacity_ == 2) {
                room.status_ = 2;
                std::cout << "gentleman enters the empty room with 2 seats" << '\n';
            }

            rooms_mutex.unlock();
            return;
        } else if (room.status_ == 1 || room.status_ == 3) {
            rooms_mutex.unlock();
            continue;
        } else if (room.status_ == 2) {
            room.status_ = 3;
            std::cout << "gentleman enters the room with gentleman" << '\n';
            rooms_mutex.unlock();
            return;
        }

        rooms_mutex.unlock();
    }

    std::cout << "gentleman can't find room tonight" << '\n';
}

/// Описывает поведение дамы
void ladyMethod() {
    for (auto &room: rooms) {
        rooms_mutex.lock();

        if (room.status_ == 0) {
            if (room.capacity_ == 1) {
                room.status_ = 3;
                std::cout << "lady      enters the empty room with 1 seat" << '\n';
            } else if (room.capacity_ == 2) {
                room.status_ = 1;
                std::cout << "lady      enters the empty room with 2 seats" << '\n';
            }

            rooms_mutex.unlock();
            return;
        } else if (room.status_ == 2 || room.status_ == 3) {
            rooms_mutex.unlock();
            continue;
        } else if (room.status_ == 1) {
            room.status_ = 3;
            std::cout << "lady      enters the room with lady" << '\n';
            rooms_mutex.unlock();
            return;
        }

        rooms_mutex.unlock();
    }

    std::cout << "lady      can't find room tonight" << '\n';
}

int readInt() {
    while (true) // цикл продолжается до тех пор, пока пользователь не введет корректное значение
    {
        int a;
        std::cin >> a;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(32767, '\n');
            std::cout << "Incorrect input. Try again.\n";
        } else {
            std::cin.ignore(32767, '\n'); // удаляем лишние значения
            return a;
        }
    }
}

int generateNum() {
    return rand() % 20 + 1;
}

int main() {
    srand(time(0));
    for (int i = 0; i < kOneCapacityRoomsCount; ++i) {
        Room room(1);
        rooms[i] = room;
    }
    for (int i = kOneCapacityRoomsCount; i < rooms_count; ++i) {
        Room room(2);
        rooms[i] = room;
    }
    std::cout << "If you want to have random, enter negative values" << '\n';
    std::cout << '\n';

    std::cout << "Enter ladies count: ";
    int ladies_count = readInt();
    std::cout << '\n';
    std::cout << "Enter gentleman count: ";
    int gentlemen_count = readInt();
    std::cout << '\n';

    if (ladies_count < 0 || gentlemen_count < 0 || ladies_count + gentlemen_count >= 54) {
        std::cout << "Ladies and gentlemen will be generated randomly" << '\n';
        ladies_count = generateNum();
        gentlemen_count = generateNum();
    }

    std::cout << "Ladies: ";
    std::cout << ladies_count << '\n';
    std::cout << "Gentlemen: ";
    std::cout << gentlemen_count << '\n';

    std::thread **threads = new std::thread *[ladies_count + gentlemen_count];
    for (int i = 0; i < gentlemen_count; ++i) {
        threads[i] = new std::thread(gentlemanMethod);
    }
    for (int i = gentlemen_count; i < ladies_count + gentlemen_count; ++i) {
        threads[i] = new std::thread(ladyMethod);
    }

    for (int i = 0; i < ladies_count + gentlemen_count; ++i) {
        threads[i]->join();
        delete threads[i];
    }

    delete[] threads;

    return 0;
}
