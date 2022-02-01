# PSiW-SOP2-Chat-Projekt

Client - Server type chat application designed to enable inter-process communication with message queues. Created for PSiW (parallel and system programing) aka SOP2 (Operating Systems) classes with dr hab. AKobusinska. 

Feel free to inspire yourself.

# Instrukcja obsługi

## Komunikacja klient - server

Każdy klient komunikuje się z serverem poprzez osobną kolejkę komunikatów - kluczem odczytu jest numer PID klienta.

## Struktura Wiadomości
* long type - klucz
* int pid - proces ID
* char[20] nick - nazwa użytkownika
* char[50] chat - nazwa kanału, w ramach którego wysyłana jest wiadomość
* char[512] content - zawartość wiadomości
* int task - rodzaj wykonywanego zadania

## Struktura Użytkownika
* int msg_id - ID użytkownika, PID klienta
* char[20] nick - nazwa użytkownika

## Struktura Kanału
* int id - ID kanału
* char[20] name - nazwa kanału
* int num_users - ilość aktywnych użytkoników w danym kanale
* FILE *fPtr - wskaznik do pliku z historią wiadomości czatu
* User *users - tablica podłączonych od czatu użytkowników

## Komunikacja klient -> server

Klient wysyła wiadomość poprzez kolejkę komunikatów. Wiadomość zawiera parametr TASK, który odpowiada jednej z opcji dostępnych w MENU. Następnie server odpowiada na zapytanie w zdefiniowany poniżej sposób.

### Task = 1
Klient wysyła zapytanie do servera. W odpowiedzi server zwraca listę wszystkich aktualnie podłączonych użytkowników.

### Task = 2
Klient wysyła zapytanie do servera, które zawiera nazwę kanału (pokoju). Jeśli kanał o takiej nazwie już istnieje, użytkownik zostaje do niego dołączony. W przeciwnym wypadku najpierw odpowiedni kanał zostanie utworzony.

###  Task = 3
Klient wysyła zapytanie do servera. W odpowiedzi server zwraca listę wszystkich aktywnych kanałów.

###  Task = 4
Klient wysyła zapytanie do servera. W odpowiedzi server zwraca listę wszystkich podłączonych użytkowników.

### Task = 5
Klient wysyła zapytanie, którego wynikiem jest usunięcie użytkownika z wybranego kanału przez serwer (odłączenie).

###  Task = 6
Klient wchodzi w tryb "czat" - łączy się z wybranym kanałem w sposób umożliwiający wysyłanie oraz odbieranie wiadomości w obrębie danego pokoju. Jednocześnie zostaje załadowana lista poprzednich wiadomości z tego kanału. W celu opuszczenia tego trybu, należy wpisać 'q'.

### Task = 7
Klient wysyła wiadomość prywatną (DM - direct message) do innego wybranego użytkownika servera.

### Task = 10
Następuje wylogowanie klienta z servera.

## Komunikacja server -> klient
Server realizuje zapytania przesłane przez klienta.

## Instrukcja kompilacji
W celu skompilowania projektu należy podłużyć się kompliatorem GCC.
gcc klient.c -o server.o
gcc server.c -o klient.o

# Instrukcja uruchomienia
Aplikacja Servera przyjmuje jego parametr pierwszy (agrv[1]) klucz kolejki.
Aplikacja Klienta przyjmuje jako parametr pierwszy (argv[1]) nazwę użytkownika, a jako parametr drugi (argv[2]) klucz kolejki servera.

Przykładowe użycie
./server.o 77777
./client.o Wojtek 77777
./client.o Mateusz 77777

## Zawartość Plików
server.c - Aplikacja servera
klient.c - Aplikacja klienta
data.h - Plik nagłówkowy zawierający struktury danych Użytkownika, Kanału, Wiadomości.
