#include <iostream>
#include <string>
#include <iomanip> // Library untuk membuat tabel yang rapi

using namespace std;

// [ALPRO-4] NAMESPACE: Mengempokan semua fungsi dan struct ke dalam namespace "SkyHub"
namespace SkyHub {

    // [ALPRO-1] STRUCT: Definisi entitas data untuk penerbangan, penumpang, staf, dan undo stack
    // [ALPRO-3] POINTER: Penggunaan '*' untuk alokasi memori dinamis dan pengelolaan linked list
    struct Flight {
        string kode, tujuan, status;
        int kapasitas;
        Flight *next, *prev; // [STRUKDAT] Pointer untuk Double Linked List
    };

    struct Passenger {
        string nama, kelas;
        int idPaspor, prioritas;
        Passenger* next; // [STRUKDAT] Pointer untuk Priority Queue
    };

    struct Staff {
        string nama;
        Staff* next; // [STRUKDAT] Pointer untuk Circular Linked List
    };

    struct UndoStack {
        string statusLama;
        UndoStack* next; // [STRUKDAT] Pointer untuk Stack
    };

    // Global Pointers
    Flight* headFlight = nullptr;           // Pointer untuk Double Linked List
    Passenger* headBoarding = nullptr;      // Pointer untuk Priority Queue
    Staff* currentStaff = nullptr;          // Pointer untuk Circular Linked List
    UndoStack* topUndo = nullptr;           // Pointer untuk Stack

    // [ALPRO-6] INLINE FUNCTION: Fungsi efisiensi tinggi
    inline bool isFlightEmpty() { return headFlight == nullptr; }

    // [ALPRO-2] REFERENCES (&): Update data asli tanpa perlu return/salinan baru
    void updateFlightStatus(string &current, string newStatus) {
        current = newStatus;
    }

    // [ALPRO-7] TEMPLATE FUNCTION: Fungsi universal untuk berbagai tipe data
    template <typename T>
    void systemLog(T message) {
        cout << "[LOG]: " << message << endl;
    }

    // [ALPRO-7] FUNCTION OVERLOADING: Nama fungsi sama dengan parameter berbeda
    void cariData(string nama) { 
        cout << "\n========================================================\n";
        cout << "\n[PENCARIAN DATABASE]";
        cout << "\nStatus: DITEMUKAN";
        cout << "\nNama Penumpang : " << nama;
        cout << "\nKeterangan     : Penumpang terdaftar dalam manifest.";
        cout << "\n=======================================================\n";
    }

    void cariData(int id) { 
        cout << "\n========================================================\n";
        cout << "\n[PENCARIAN DATABASE]";
        cout << "\nStatus: TERVERIFIKASI";
        cout << "\nID Paspor      : " << id;
        cout << "\nKeterangan     : Dokumen valid untuk keberangkatan.";
        cout << "\n=======================================================\n";
    }

    // [ALPRO-5] CALLBACK FUNCTION: Fungsi yang diteruskan sebagai parameter filter penerbangan
    void filterPenerbangan(void (*logic)(Flight*)) {
        Flight* temp = headFlight;
        while (temp) {
            logic(temp);
            temp = temp->next;
        }
    }

    // [STRUKDAT] DISPLAY: Fungsi untuk menampilkan data dalam format tabel
    void tampilkanTabelFlight() {
        if (isFlightEmpty()) return;
        cout << "\n" << setfill('=') << setw(66) << "=" << endl;
        cout << setfill(' ');
        cout << "| " << setw(10) << left << "KODE" 
             << "| " << setw(15) << "TUJUAN" 
             << "| " << setw(12) << "STATUS" 
             << "| " << setw(10) << "KAPASITAS" << " |" << endl;
        cout << setfill('=') << setw(66) << "=" << endl;
        cout << setfill(' ');
        Flight* temp = headFlight;
        while (temp) {
            cout << "| " << setw(10) << temp->kode 
                 << "| " << setw(15) << temp->tujuan 
                 << "| " << setw(12) << temp->status 
                 << "| " << setw(10) << temp->kapasitas << " |" << endl;
            temp = temp->next;
        }
        cout << setfill('=') << setw(66) << "=" << endl;
    }

    void tampilkanTabelBoarding() {
        if (!headBoarding) { cout << "Belum ada antrean boarding.\n"; return; }
        cout << "\n" << setfill('=') << setw(55) << "=" << endl;
        cout << setfill(' ');
        cout << "| " << setw(5) << left << "PRIO" 
             << "| " << setw(15) << "NAMA" 
             << "| " << setw(12) << "KELAS" 
             << "| " << setw(10) << "PASPOR" << " |" << endl;
        cout << setfill('=') << setw(55) << "=" << endl;
        cout << setfill(' ');
        Passenger* temp = headBoarding;
        while (temp) {
            cout << "| " << setw(5) << temp->prioritas 
                 << "| " << setw(15) << temp->nama 
                 << "| " << setw(12) << temp->kelas 
                 << "| " << setw(10) << temp->idPaspor << " |" << endl;
            temp = temp->next;
        }
        cout << setfill('=') << setw(55) << "=" << endl;
    }

    void startFilterTable() {
    cout << "\n" << setfill('-') << setw(50) << "-" << endl;
    cout << setfill(' ');
    cout << "| " << setw(15) << left << "KODE" 
         << "| " << setw(20) << "TUJUAN" 
         << "| " << setw(10) << "STATUS" << " |" << endl;
    cout << setfill('-') << setw(50) << "-" << endl;
    }

    void cekHanyaDelay(SkyHub::Flight* f) {
    if (f->status == "Delay" || f->status == "delay") {
        cout << setfill(' ');
        cout << "| " << setw(15) << f->kode 
             << "| " << setw(20) << f->tujuan 
             << "| " << setw(10) << f->status << " |" << endl;
        }
    }

    void endFilterTable() {                                     // Fungsi untuk menutup tabel setelah filter
        cout << setfill('-') << setw(50) << "-" << endl;
    }
    
    // [STRUKDAT] INSERT & ENQUEUE: Fungsi untuk penambahan data ke struktur data yang sesuai
    void tambahJadwal(string k, string t, string s, int kap) {                      // [STRUKDAT] Double Linked List: Tambah jadwal penerbangan baru
        Flight* newNode = new Flight{k, t, s, kap, nullptr, nullptr};
        if (isFlightEmpty()) headFlight = newNode;
        else {
            Flight* temp = headFlight;
            while (temp->next) temp = temp->next;
            temp->next = newNode; newNode->prev = temp;
        }
    }

    void enqueueBoarding(string n, int id, string k, int p) {                       // [STRUKDAT] Priority Queue: Penumpang dengan prioritas lebih tinggi (nilai p lebih kecil) akan berada di depan
        Passenger* newNode = new Passenger{n, k, id, p, nullptr};
        if (!headBoarding || p < headBoarding->prioritas) {
            newNode->next = headBoarding;
            headBoarding = newNode;
        } else {
            Passenger* temp = headBoarding;
            while (temp->next && temp->next->prioritas <= p) temp = temp->next;
            newNode->next = temp->next; temp->next = newNode;
        }
    }

    // [STRUKDAT] CIRCULAR LINKED LIST: Fungsi untuk menambah staf dengan siklus berputar
    void tambahStaff(string n) {
        Staff* newNode = new Staff{n, nullptr};
        if (!currentStaff) {
            currentStaff = newNode;
            newNode->next = currentStaff;
        } else {
            newNode->next = currentStaff->next;
            currentStaff->next = newNode;
        }
    }

    void pushUndo(string s) {                       // [STRUKDAT] STACK: Menyimpan status lama sebelum update untuk memungkinkan undo
        topUndo = new UndoStack{s, topUndo};
    }
}

// Logika Callback untuk filter pesawat bermasalah (delay)
void cekHanyaDelay(SkyHub::Flight* f) {                         
    if (f->status == "Delay" || f->status == "delay") {
        cout << "> Info: Pesawat " << f->kode << " ke " << f->tujuan << " sedang DELAY." << endl;
    }
}

// =========================================================
// MAIN PROGRAM
// =========================================================
int main() {
    // Inisialisasi awal staf
    SkyHub::tambahStaff("Doni Drian (Gate 1)");         
    SkyHub::tambahStaff("Lusi Mishima (Gate 2)");
    
    // DEKLARASI VARIABEL DI ATAS (Biar case 6 dan lainnya bersih)
    int pilihan, id, p, kap, subOpt, cariId;
    string k, t, s, nama, kelas, cariStr;

    do {
        cout << "\n================================================";
        cout << "\n   SKYHUB: SISTEM BANDARA INTERNASIONAL v1.0   ";
        cout << "\n================================================";
        cout << "\n1. Tambah Jadwal Pesawat (Double LL)";
        cout << "\n2. Update Status & Simpan Log (Reference & Stack)";
        cout << "\n3. Boarding Penumpang (Priority Queue)";
        cout << "\n4. Cek Pesawat Delay (Callback Function)";
        cout << "\n5. Rotasi Petugas Gate (Circular LL)";
        cout << "\n6. Cari Data Penumpang (Overloading)";
        cout << "\n0. Keluar";
        cout << "\nPilihan: ";
        cin >> pilihan;

        switch (pilihan) {
            case 1:
                cout << "Kode: "; cin >> k;
                cout << "Tujuan: "; cin >> t;
                cout << "Status: "; cin >> s;
                cout << "Kapasitas: "; cin >> kap;
                SkyHub::tambahJadwal(k, t, s, kap);
                SkyHub::systemLog("Jadwal " + k + " berhasil ditambahkan.");
                SkyHub::tampilkanTabelFlight();
                break;

            case 2:
                if (SkyHub::isFlightEmpty()) {
                    cout << "Daftar penerbangan kosong!\n";
                } else {
                    SkyHub::pushUndo(SkyHub::headFlight->status); // [STRUKDAT] Push
                    cout << "Ubah status " << SkyHub::headFlight->kode << " jadi: "; cin >> s;
                    SkyHub::updateFlightStatus(SkyHub::headFlight->status, s);
                    cout << "Status berhasil diupdate!\n";
                    SkyHub::tampilkanTabelFlight();
                }
                break;

            case 3:
                cout << "Nama: "; cin >> nama;
                cout << "ID Paspor: "; cin >> id;
                cout << "Kelas (FirstClass/Business/Economy): "; cin >> kelas;
                cout << "Prioritas (1-3): "; cin >> p;
                SkyHub::enqueueBoarding(nama, id, kelas, p);        // [STRUKDAT] Enqueue
                cout << "Penumpang masuk antrean boarding.\n";      
                SkyHub::tampilkanTabelBoarding();                   // [STRUKDAT] Display 
                break;

            case 4:
                cout << "\n--- DAFTAR PENERBANGAN DELAY ---";
                // Header Tabel Filter
                cout << "\n" << setfill('-') << setw(45) << "-" << endl;
                cout << setfill(' ') << "| " << setw(10) << left << "KODE" << "| " << setw(15) << "TUJUAN" << "| " << setw(12) << "STATUS" << " |" << endl;
                cout << setfill('-') << setw(45) << "-" << setfill(' ') << endl;
                SkyHub::filterPenerbangan(cekHanyaDelay);              // [STRUKDAT] Callback Function
                cout << setfill('-') << setw(45) << "-" << endl;
                break;

            case 5:
                if (SkyHub::currentStaff) {
                    cout << "Petugas Aktif: " << SkyHub::currentStaff->nama << endl;
                    SkyHub::currentStaff = SkyHub::currentStaff->next;                      // [STRUKDAT] Circular Linked List
                    cout << "Petugas Berikutnya: " << SkyHub::currentStaff->nama << endl;
                }
                break;

            case 6:
                // Sekarang Case 6 bersih tanpa deklarasi variabel di dalam
                cout << "Cari berdasarkan:\n1. Nama Penumpang\n2. ID Paspor\nPilihan: ";
                cin >> subOpt;
                if (subOpt == 1) {
                    cout << "Masukkan Nama: "; cin >> cariStr;
                    SkyHub::cariData(cariStr);      // [STRUKDAT] Function Overloading String
                } else {
                    cout << "Masukkan ID Paspor: "; cin >> cariId;
                    SkyHub::cariData(cariId);       // [STRUKDAT] Function Overloading Int
                }
                break;

            case 0:
                cout << "Sistem dimatikan. Sampai jumpa!\n";
                break;

            default:
                cout << "Pilihan tidak valid!\n";
                break;
        }
    } while (pilihan != 0);

    return 0;
}
