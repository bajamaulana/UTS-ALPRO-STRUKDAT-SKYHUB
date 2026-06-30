#include <iostream>
#include <string>
#include <iomanip>
#include <vector>
#include <queue>
#include <stack>
#include <unordered_map>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <functional>
#include <stdexcept>
#include <limits>

using namespace std;










struct user {
    string username;
    string password;
    string role;
};


namespace SkyHub {
    // =========================================================================
    // [STRUKDAT-1] DOUBLE LINKED LIST (Sisi Operator Maskapai - Manifes Jadwal)
    // =========================================================================
    struct Penerbangan {
        string kodePenerbangan;
        string kotaTujuan;
        string jamKeberangkatan; // [ALPRO-1] TIPE DATA STRING UNTUK FORMAT JAM '08.00'
        Penerbangan* next;
        Penerbangan* prev;
    };

    // =========================================================================
    // [STRUKDAT-2] CIRCULAR LINKED LIST (Sisi Otoritas Bandara - Shift Petugas)
    // =========================================================================
    struct PetugasBandara {
        string namaPetugas;
        string divisiKerja;
        PetugasBandara* next;
    };

    // =========================================================================
    // [STRUKDAT-3] STACK (Sisi Otoritas Bandara - Undo Log Modifikasi Operasional)
    // =========================================================================
    enum TipeAksi { TAMBAH_JADWAL, HAPUS_JADWAL, LAINNYA };

    struct UndoAction {
        TipeAksi aksi;
        string statusLama;
        string kode;
        string kota;
        string jam;
        UndoAction* next;
    };

    // =========================================================================
    // [STRUKDAT-4] AVL TREE (Sisi Operator Maskapai - Manifes Paspor Penumpang)
    // =========================================================================
    struct PenumpangNode {
        string nomorPaspor;
        string namaLengkap;
        string kodeBookingPNR;
        int height; // Atribut tinggi pohon untuk syarat balancing
        PenumpangNode* left;
        PenumpangNode* right;
        PenumpangNode(string paspor, string nama, string pnr) 
            : nomorPaspor(paspor), namaLengkap(nama), kodeBookingPNR(pnr), height(1), left(nullptr), right(nullptr) {}
    };

    // =========================================================================
    // [STRUKDAT-5] PRIORITY QUEUE STL (Sisi Otoritas Bandara - Antrean Runway ATC)
    // =========================================================================
    struct RunwayItem {
        string kodePenerbangan;
        int tingkatUrgensi; // 1 = Reguler, 2 = Darurat Medis, 3 = Gangguan Mesin
        // [ALPRO-5] OPERATOR OVERLOADING UNTUK MENENTUKAN PRIORITAS ANTRIAN
        bool operator<(const RunwayItem& other) const {
            return this->tingkatUrgensi < other.tingkatUrgensi;
        }
    };

    // [ALPRO-2] POINTER GLOBAL AKSES MEMORI RAM
    Penerbangan* headPenerbangan = nullptr;
    PetugasBandara* currentShiftPetugas = nullptr;
    
    // =========================================================================
    // PERBAIKAN LOGIKA UNDO: Memisahkan Stack Tunggal Menjadi Dua Jalur Role
    // =========================================================================
    UndoAction* undoStackMaskapai = nullptr; 
    UndoAction* undoStackBandara = nullptr;  
    
    PenumpangNode* rootPassengerAVL = nullptr;
      vector<string> arsipKeberangkatan = {};

    priority_queue<RunwayItem> runwayPriorityQueue;
    unordered_map<string, string> informasiGerbangHashMap; // [STRUKDAT-6] HASH TABLE O(1)
    unordered_map<string, vector<string>> navigasiRuteGraph; // [STRUKDAT-7] GRAPH ADJACENCY LIST







    // [ALPRO-3] INLINE FUNCTION UNTUK PENGECEKAN KONDISI BASIS
    inline bool isJadwalEmpty() { return headPenerbangan == nullptr; }
    
    // [ALPRO-4] PASS BY REFERENCE (&) & [ALPRO-6] DEFAULT ARGUMENT
    void updateStatusOperasional(string &currentStatus, string statusBaru = "STANDBY") {
        currentStatus = statusBaru;
        cout << "[UPDATE STATUS] Status operasional diubah menjadi: " << currentStatus << endl;
    }

    // [ALPRO-6] FUNCTION OVERLOADING - VERSI 1 (MENERIMA PARAMETER STRING)
    void cariData(string detailLokasi) {
        cout << "\n[PROGRES HASH MAP] Lokasi Gerbang Sukses Ditemukan: " << detailLokasi << endl;
    }
    
    // [ALPRO-6] FUNCTION OVERLOADING - VERSI 2 (MENERIMA PARAMETER INTEGER)
    void cariData(int statusTree) {
        if(statusTree == 1) cout << "\n[PROGRES AVL TREE] Paspor Penumpang Berhasil Terindeks di Pohon Seimbang." << endl;
    }

    void tambahJadwalPenerbangan(string kode, string tujuan, string jam) {
        Penerbangan* newNode = new Penerbangan{kode, tujuan, jam, nullptr, nullptr};
        if (isJadwalEmpty()) {
            headPenerbangan = newNode;
        } else {
            Penerbangan* temp = headPenerbangan;
            while (temp->next) temp = temp->next;
            temp->next = newNode;
            newNode->prev = temp;
        }
    }

    void hapusJadwalPenerbangan(string kode) {
        if (isJadwalEmpty()) {
            cout << "Jadwal kosong, tidak ada yang bisa dihapus.\n";
            return;
        }
        Penerbangan* temp = headPenerbangan;
        while (temp) {
            if (temp->kodePenerbangan == kode) {
                if (temp->prev) temp->prev->next = temp->next;
                else headPenerbangan = temp->next;
                if (temp->next) temp->next->prev = temp->prev;
                delete temp;
                cout << "[DOUBLE LL DELETE] Jadwal pesawat " << kode << " berhasil dihapus.\n";
                return;
            }
            temp = temp->next;
        }
        cout << "Jadwal pesawat dengan kode " << kode << " tidak ditemukan.\n";
    }

    void tambahPetugasBandara(string nama, string divisi) {
        PetugasBandara* newNode = new PetugasBandara{nama, divisi, nullptr};
        if (!currentShiftPetugas) {
            currentShiftPetugas = newNode;
            newNode->next = currentShiftPetugas;
        } else {
            newNode->next = currentShiftPetugas->next;
            currentShiftPetugas->next = newNode;
        }
    }

    void cetakPetugasShift() {
        if (!currentShiftPetugas) {
            cout << "Belum ada petugas shift terdaftar.\n";
            return;
        }
        PetugasBandara* temp = currentShiftPetugas;
        cout << "Daftar Petugas Shift Saat Ini:\n";
        do {
            cout << "- Nama: " << temp->namaPetugas << ", Divisi: " << temp->divisiKerja << endl;
            temp = temp->next;
        } while (temp != currentShiftPetugas);
    }

    void hapusPetugas(string nama) {
        if (!currentShiftPetugas) {
            cout << "Belum ada petugas shift terdaftar.\n";
            return;
        }
        PetugasBandara* curr = currentShiftPetugas;
        do {
            if (curr->namaPetugas == nama) {
                if (curr->next == curr) {
                    delete curr;
                    currentShiftPetugas = nullptr;
                    cout << "[CIRCULAR LL DELETE] Petugas " << nama << " telah dihapus.\n";
                    return;
                }
                PetugasBandara* temp = currentShiftPetugas;
                while (temp->next != curr) temp = temp->next;
                temp->next = curr->next;
                if (currentShiftPetugas == curr) currentShiftPetugas = curr->next;
                delete curr;
                cout << "[CIRCULAR LL DELETE] Petugas " << nama << " telah dihapus.\n";
                return;
            }
            curr = curr->next;
        } while (curr != currentShiftPetugas);
        cout << "Petugas tidak ditemukan.\n";
    }

    // Mekanisme Push Tumpukan Stack Manual (fitur undo) - Ditambahkan Parameter Reference Pointer Stack
    void pushUndoLog(UndoAction* &topStack, string statusLama) {
        topStack = new UndoAction{LAINNYA, statusLama, "", "", "", topStack};
    }

    void pushUndoLog(UndoAction* &topStack, TipeAksi aksi, string statusLama, string kode, string kota, string jam) {
        topStack = new UndoAction{aksi, statusLama, kode, kota, jam, topStack};
    }

    void popUndoLog(UndoAction* &topStack) {
        if (!topStack) {
            cout << "Undo Log Kosong! Tidak ada aksi yang bisa dibatalkan untuk menu ini.\n";
            return;
        }
        UndoAction* temp = topStack;
        string statusRevert = temp->statusLama;
        
        if (temp->aksi == TAMBAH_JADWAL) {
            hapusJadwalPenerbangan(temp->kode);
            cout << "[UNDO] Jadwal " << temp->kode << " ditarik kembali.\n";
        } else if (temp->aksi == HAPUS_JADWAL) {
            tambahJadwalPenerbangan(temp->kode, temp->kota, temp->jam);
            cout << "[UNDO] Jadwal " << temp->kode << " dikembalikan ke sistem.\n";
        } else {
            cout << "[STACK POP] Undo berhasil. Status dikembalikan (Revert aksi): " << statusRevert << "\n";
        }
        
        topStack = topStack->next;
        delete temp;
    }



    // Logika Rotasi Penyeimbang Struktur Data Non-Linear AVL Tree (fitur manifes penumpang)
    int getHeight(PenumpangNode* n) { return n ? n->height : 0; }
    int getBalanceFactor(PenumpangNode* n) { return n ? getHeight(n->left) - getHeight(n->right) : 0; }

    PenumpangNode* rightRotate(PenumpangNode* y) {
        PenumpangNode* x = y->left;
        PenumpangNode* T2 = x->right;
        x->right = y;
        y->left = T2;
        y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
        x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
        return x;
    }

    PenumpangNode* leftRotate(PenumpangNode* x) {
        PenumpangNode* y = x->right;
        PenumpangNode* T2 = y->left;
        y->left = x;
        x->right = T2;
        x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
        y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
        return y;
    }

    PenumpangNode* insertPassenger(PenumpangNode* node, string paspor, string nama, string pnr) {
        if (!node) return new PenumpangNode(paspor, nama, pnr);
        if (paspor < node->nomorPaspor) node->left = insertPassenger(node->left, paspor, nama, pnr);
        else if (paspor > node->nomorPaspor) node->right = insertPassenger(node->right, paspor, nama, pnr);
        else return node;

        node->height = 1 + max(getHeight(node->left), getHeight(node->right));
        int balance = getBalanceFactor(node);

        if (balance > 1 && paspor < node->left->nomorPaspor) return rightRotate(node);
        if (balance < -1 && paspor > node->right->nomorPaspor) return leftRotate(node);
        if (balance > 1 && paspor > node->left->nomorPaspor) {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }
        if (balance < -1 && paspor < node->right->nomorPaspor) {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }
        return node;
    }

    void cetakDataPassenggerInOrder(PenumpangNode* node) {
        if (!node) return;
        cetakDataPassenggerInOrder(node->left);
        cout << "Nomor Paspor: " << node->nomorPaspor << ", Nama: " << node->namaLengkap << ", PNR: " << node->kodeBookingPNR << endl;
        cetakDataPassenggerInOrder(node->right);
    }

    PenumpangNode* minValueNode(PenumpangNode* node) {
        PenumpangNode* current = node;
        while (current->left != nullptr) current = current->left;
        return current;
    }

    PenumpangNode* deletePassenger(PenumpangNode* root, string paspor) {
        if (root == nullptr) return root;

        if (paspor < root->nomorPaspor)
            root->left = deletePassenger(root->left, paspor);
        else if (paspor > root->nomorPaspor)
            root->right = deletePassenger(root->right, paspor);
        else {
            if ((root->left == nullptr) || (root->right == nullptr)) {
                PenumpangNode* temp = root->left ? root->left : root->right;
                if (temp == nullptr) {
                    temp = root;
                    root = nullptr;
                } else
                    *root = *temp; 
                delete temp;
            } else {
                PenumpangNode* temp = minValueNode(root->right);
                root->nomorPaspor = temp->nomorPaspor;
                root->namaLengkap = temp->namaLengkap;
                root->kodeBookingPNR = temp->kodeBookingPNR;
                root->right = deletePassenger(root->right, temp->nomorPaspor);
            }
        }

        if (root == nullptr) return root;

        root->height = 1 + max(getHeight(root->left), getHeight(root->right));
        int balance = getBalanceFactor(root);

        if (balance > 1 && getBalanceFactor(root->left) >= 0) return rightRotate(root);
        if (balance > 1 && getBalanceFactor(root->left) < 0) {
            root->left = leftRotate(root->left);
            return rightRotate(root);
        }
        if (balance < -1 && getBalanceFactor(root->right) <= 0) return leftRotate(root);
        if (balance < -1 && getBalanceFactor(root->right) > 0) {
            root->right = rightRotate(root->right);
            return leftRotate(root);
        }

        return root;
    }

    // [ALPRO-7] RECURSIVE FUNCTION UNTUK PENCARIAN ELEMEN POHON AVL TREE
    PenumpangNode* searchPassenger(PenumpangNode* root, string paspor) {
        if (!root || root->nomorPaspor == paspor) return root; // Base Case Rekursif
        if (root->nomorPaspor < paspor) return searchPassenger(root->right, paspor);
        return searchPassenger(root->left, paspor);
    }

    void addRutePenerbangan(string asal, string tujuan) {
        navigasiRuteGraph[asal].push_back(tujuan);
        navigasiRuteGraph[tujuan].push_back(asal);
    }

    void traverseBFS(string startNode) {
        if(navigasiRuteGraph.find(startNode) == navigasiRuteGraph.end()) return;
        unordered_map<string, bool> visited;
        queue<string> q;
        visited[startNode] = true;
        q.push(startNode);
        cout << "Rute Transit Minimum Jaringan Udara (BFS): ";
        while(!q.empty()) {
            string curr = q.front(); q.pop();
            cout << curr << " -> ";
            for(const auto& neighbor : navigasiRuteGraph[curr]) {
                if(!visited[neighbor]) {
                    visited[neighbor] = true;
                    q.push(neighbor);
                }
            }
        }
        cout << "SELESAI\n";
    }

    void traverseDFS(string startNode) {
        if(navigasiRuteGraph.find(startNode) == navigasiRuteGraph.end()) return;
        unordered_map<string, bool> visited;
        stack<string> s;
        s.push(startNode);
        cout << "Rute Penelusuran Udara Mendalam (DFS): ";
        while(!s.empty()) {
            string curr = s.top(); s.pop();
            if(!visited[curr]) {
                cout << curr << " -> ";
                visited[curr] = true;
                for(const auto& neighbor : navigasiRuteGraph[curr]) {
                    if(!visited[neighbor]) {
                        s.push(neighbor);
                    }
                }
            }
        }
        cout << "SELESAI\n";
    }

    void cetakTabelJadwal() {
        if (isJadwalEmpty()) return;
        cout << "\n=========================================================\n";
        cout << "| KODE PESAWAT | KOTA TUJUAN        | JAM KEBERANGKATAN |\n";
        cout << "=========================================================\n";
        Penerbangan* temp = headPenerbangan;
        while (temp) {
            cout << "| " << setw(13) << temp->kodePenerbangan << "| " << setw(19) << temp->kotaTujuan << "| " << setw(18) << temp->jamKeberangkatan << " |\n";
            temp = temp->next;
        }
        cout << "=========================================================\n";
    }

    // [ALPRO-5] CALLBACK FUNCTION & [ALPRO-9] ITERATOR COUNT DINAMIS
    void auditLogPenerbangan(function<bool(const string&)> kriteriaCallback, const vector<string>& arsip) {
        int total = count_if(arsip.begin(), arsip.end(), kriteriaCallback);
        cout << "[STL COUNT & CALLBACK] Ditemukan " << total << " riwayat penerbangan yang sesuai dengan input Anda.\n";
    }

    void simpanAVLTreePreOrder(PenumpangNode* node, ofstream& out) {
        if (!node) return;
        out << node->nomorPaspor << "," << node->namaLengkap << "," << node->kodeBookingPNR << "\n";
        simpanAVLTreePreOrder(node->left, out);
        simpanAVLTreePreOrder(node->right, out);
    }


    void simpanSemuaData() {
        cout << "[FILE HANDLING] Auto-Saving Data ke Disk...\n";
        
        ofstream fileJadwal("data_jadwal.txt");
        Penerbangan* tempJ = headPenerbangan;
        while (tempJ) {
            fileJadwal << tempJ->kodePenerbangan << "," << tempJ->kotaTujuan << "," << tempJ->jamKeberangkatan << "\n";
            tempJ = tempJ->next;
        }
        fileJadwal.close();

        ofstream filePassenger("data_penumpang.txt");
        simpanAVLTreePreOrder(rootPassengerAVL, filePassenger);
        filePassenger.close();

        ofstream filePetugas("data_petugas.txt");
        if (currentShiftPetugas) {
            PetugasBandara* tempP = currentShiftPetugas;
            do {
                filePetugas << tempP->namaPetugas << "," << tempP->divisiKerja << "\n";
                tempP = tempP->next;
            } while (tempP != currentShiftPetugas);
        }
        filePetugas.close();

        ofstream fileArsip("data_arsip.txt");
        for (const auto& m : arsipKeberangkatan) {
            fileArsip << m << "\n";
        }
        fileArsip.close();

        ofstream fileGerbang("data_gerbang.txt");
        for (auto const& pair : informasiGerbangHashMap) {
            fileGerbang << pair.first << "," << pair.second << "\n";
        }
        fileGerbang.close();

        ofstream fileRute("data_rute.txt");
        for (auto const& pair : navigasiRuteGraph) {
            for (const auto& dest : pair.second) {
                fileRute << pair.first << "," << dest << "\n";
            }
        }
        fileRute.close();
        
        cout << "[FILE HANDLING] Seluruh data berhasil disimpan secara permanen!\n";
    }

    void loadSemuaData() {
        cout << "[FILE HANDLING] Memuat Data dari Disk...\n";
        string line, part1, part2, part3;
        
        ifstream fileJadwal("data_jadwal.txt");
        if(fileJadwal.is_open()) {
            while(getline(fileJadwal, line)) {
                stringstream ss(line);
                if(getline(ss, part1, ',') && getline(ss, part2, ',') && getline(ss, part3, ',')) {
                    tambahJadwalPenerbangan(part1, part2, part3);
                }
            }
            fileJadwal.close();
        }

        ifstream filePenumpang("data_penumpang.txt");
        if(filePenumpang.is_open()) {
            while(getline(filePenumpang, line)) {
                stringstream ss(line);
                if(getline(ss, part1, ',') && getline(ss, part2, ',') && getline(ss, part3, ',')) {
                    rootPassengerAVL = insertPassenger(rootPassengerAVL, part1, part2, part3);
                }
            }
            filePenumpang.close();
        }

        ifstream filePetugas("data_petugas.txt");
        bool isPetugasEmpty = true;
        if(filePetugas.is_open()) {
            while(getline(filePetugas, line)) {
                stringstream ss(line);
                if(getline(ss, part1, ',') && getline(ss, part2, ',')) {
                    tambahPetugasBandara(part1, part2);
                    isPetugasEmpty = false;
                }
            }
            filePetugas.close();
        }

        ifstream fileArsip("data_arsip.txt");
        if(fileArsip.is_open()) {
            arsipKeberangkatan.clear();
            while(getline(fileArsip, line)) {
                if(!line.empty()) {
                    arsipKeberangkatan.push_back(line);
                }
            }
            fileArsip.close();
        }

        ifstream fileGerbang("data_gerbang.txt");
        if(fileGerbang.is_open()) {
            while(getline(fileGerbang, line)) {
                stringstream ss(line);
                if(getline(ss, part1, ',') && getline(ss, part2, ',')) {
                    informasiGerbangHashMap[part1] = part2;
                }
            }
            fileGerbang.close();
        }

        ifstream fileRute("data_rute.txt");
        bool isRuteEmpty = true;
        if(fileRute.is_open()) {
            while(getline(fileRute, line)) {
                stringstream ss(line);
                if(getline(ss, part1, ',') && getline(ss, part2, ',')) {
                    bool exist = false;
                    for(auto neighbor : navigasiRuteGraph[part1]) {
                        if(neighbor == part2) exist = true;
                    }
                    if(!exist) addRutePenerbangan(part1, part2);
                    isRuteEmpty = false;
                }
            }
            fileRute.close();
        }
        if(isRuteEmpty) {
            addRutePenerbangan("Bandara_Soekarno_Hatta", "Bandara_Changi_Singapore");
            addRutePenerbangan("Bandara_Changi_Singapore", "Bandara_Haneda_Tokyo");
        }

        cout << "[FILE HANDLING] Load Selesai!\n";
    }






    void bersihkanMemoriGlobal() {
        cout << "\n[MEMORY MANAGEMENT] Membersihkan sisa pointer memori dari RAM...\n";
        while (headPenerbangan) {
            Penerbangan* temp = headPenerbangan;
            headPenerbangan = headPenerbangan->next;
            delete temp;
        }
        if (currentShiftPetugas) {
            PetugasBandara* curr = currentShiftPetugas->next;
            while (curr != currentShiftPetugas) {
                PetugasBandara* temp = curr;
                curr = curr->next;
                delete temp;
            }
            delete currentShiftPetugas;
            currentShiftPetugas = nullptr;
        }
        
        // Membersihkan tumpukan Stack milik Maskapai
        while (undoStackMaskapai) {
            UndoAction* temp = undoStackMaskapai;
            undoStackMaskapai = undoStackMaskapai->next;
            delete temp;
        }
        
        // Membersihkan tumpukan Stack milik Bandara
        while (undoStackBandara) {
            UndoAction* temp = undoStackBandara;
            undoStackBandara = undoStackBandara->next;
            delete temp;
        }
        cout << "[MEMORY MANAGEMENT] Seluruh memori pointer telah dikembalikan dengan aman (Bebas Memory Leak)!\n";
    }




    // fungsi login user (Maskapai/Bandara) dengan validasi dari file eksternal
    string loginuser() {
        ifstream file("datauser.txt");
        if (!file.is_open()) {
            cout << "[FILE ERROR] datauser.txt tidak ditemukan.\n";
            return "";
        }
        string username, password;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Bersihkan sisa newline
        cout << "Masukkan Username: "; getline(cin, username);
        cout << "Masukkan Password: "; getline(cin, password);
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string fileUsername, filePassword, fileRole;

            if (getline(ss, fileUsername, ',') && 
                getline(ss, filePassword, ',') && 
                getline(ss, fileRole, ',')) {
                
                if (fileUsername == username && filePassword == password && (fileRole == "Maskapai" || fileRole == "Bandara")) {
                    cout << "Login Berhasil! Selamat datang, " << username << "!\n";
                    return fileRole;
                }
            }
        }
        cout << "Login Gagal! Username atau Password salah.\n";
        return ""; 
    }

    void daftarUserBaru() {
        string username, password, role;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Bersihkan sisa newline
        cout << "Masukkan Username Baru: "; getline(cin, username);
        cout << "Masukkan Password Baru: "; getline(cin, password);
        cout << "Masukkan Role (Maskapai/Bandara): "; cin >> role;

        ifstream fileCheck("datauser.txt");
        string line;
        bool usernameExists = false;
        if(fileCheck.is_open()) {
            while (getline(fileCheck, line)) {
                stringstream ss(line);
                string fileUsername, filePassword, fileRole;
                if (getline(ss, fileUsername, ',') && getline(ss, filePassword, ',') && getline(ss, fileRole, ',')) {
                    if (fileUsername == username) {
                        usernameExists = true;
                        break;
                    }
                }
            }
            fileCheck.close();
        }

        if (usernameExists) {
            cout << "Username sudah digunakan! Silakan pilih username lain.\n";
            return;
        } 

        ofstream file("datauser.txt", ios::app);
        if (role != "Maskapai" && role != "Bandara") {
            cout << "Role tidak valid! Harus 'Maskapai' atau 'Bandara'.\n";
            return;
        }
        file << username << "," << password << "," << role << "\n";
        cout << "Pendaftaran User Baru Berhasil! Anda dapat login dengan akun ini.\n";
        file.close();
    }


    // fungsi validasi input menu dengan try-catch untuk menangani kesalahan input
    int dapatkanPilihanMenu(int jmlhmenu) {
    int pilihan = -1;

    while (true) {
        std::cout << "Pilih menu" << " (0-" << jmlhmenu << "): ";
        
        try {
            // 1. Cek apakah input stream gagal (misal kemasukan huruf)
            if (!(std::cin >> pilihan)) {
                std::cin.clear(); // Setel ulang status fail
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Buang sisa huruf di buffer
                
                throw std::invalid_argument("Input harus berupa angka! Huruf atau simbol tidak diperbolehkan.");
            }

            // 2. Cek apakah angka yang dimasukkan ada di dalam menu
            if (pilihan < 0 || pilihan > jmlhmenu) {
                throw std::out_of_range("Nomor menu tidak tersedia! Silakan pilih angka 0 hingga " + std::to_string(jmlhmenu) + ".");
            }

            // Jika berhasil lolos dari kedua throw di atas, kembalikan nilai pilihan
            return pilihan;

        } 
        // Tangkap error huruf di sini
        catch (const std::invalid_argument& e) {
            std::cerr << "\n[ERROR INPUT]: " << e.what() << "\n\n";
            // continue di sini akan membuat program melompat ke awal 'while (true)' fungsi ini
            continue; 
        } 
        // Tangkap error angka salah di sini
        catch (const std::out_of_range& e) {
            std::cerr << "\n[ERROR MENU]: " << e.what() << "\n\n";
            continue;
        }
    }
}



}



int main() {
    int loginChoice, pilihanMenu, subMenu;
    string kode, kota, paspor, nama, pnr, detailGerbang, jamTerbang; // jamTerbang sukses diubah ke string
    int jmlhmenu;
    // Panggil fungsi Master File Handling
    SkyHub::loadSemuaData();

    do {
        cout << "===========================================================\n";
        cout << "       WELCOME TO SKYHUB CENTRAL OPERATIONAL SYSTEM        \n";
        cout << "===========================================================\n";
        cout << "Pilih menu:\n";
        cout << "1. Login User\n";
        cout << "2. Daftar User Baru\n";
        cout << "0. Keluar Program\n";
        cout << "Pilihan Anda: ";
        jmlhmenu = 2; // Jumlah menu login
        loginChoice = SkyHub::dapatkanPilihanMenu(jmlhmenu); // Panggil fungsi validasi input menu

        if (loginChoice == 1){
            string roleAccess = SkyHub::loginuser();
            if (roleAccess.empty()) {
                cout << "akses tidak valid, silakan coba lagi.\n\n";
                continue; // Keluar jika login gagal
            }

            if (roleAccess == "Maskapai") {
                // [ALPRO-9] DASHBOARD OPERATOR MASKAPAI
                do {
                    cout << "\n--- DASHBOARD OPERATOR MASKAPAI  ---";
                    cout << "\n1. Daftarkan Jadwal Penerbangan Baru (Double LL)";
                    cout << "\n2. Registrasi Manifes Paspor Penumpang (AVL Tree)";
                    cout << "\n3. Petakan Informasi Lokasi Gerbang Pesawat (Hash Table)";
                    cout << "\n4. Hapus Jadwal Penerbangan (Double LL Delete)";
                    cout << "\n5. Hapus Penumpang (AVL Tree Delete)";
                    cout << "\n6. Batalkan Aksi Terakhir / Undo (Stack Pop)";
                    cout << "\n0. logout Portal";
                    cout << "\nPilihan Operasi Maskapai: ";
                
                    // Validasi Input Menu Maskapai
                    jmlhmenu = 6; // Jumlah menu maskapai sukses dinaikkan menjadi 6 untuk menampung menu undo
                    pilihanMenu = SkyHub::dapatkanPilihanMenu(jmlhmenu);

                    switch (pilihanMenu) {
                        case 1:
                            cout << "Masukkan Kode Pesawat     : "; cin >> kode;
                            cout << "Masukkan Kota Tujuan      : "; cin >> kota;
                            cout << "Masukkan Jam Keberangkatan (Format JJ.MM): "; cin >> jamTerbang; // Berhasil menerima input format titik '08.00'
                            SkyHub::tambahJadwalPenerbangan(kode, kota, jamTerbang);
                            SkyHub::pushUndoLog(SkyHub::undoStackMaskapai, SkyHub::TAMBAH_JADWAL, "Penambahan jadwal kode " + kode, kode, kota, jamTerbang);
                            
                            // [INTEGRASI DATA] LINTAS OTORITAS: Mengisi Simpul Baru Jaringan Graph Bandara secara Otomatis
                            SkyHub::addRutePenerbangan("Bandara_Soekarno_Hatta", "Bandara_" + kota);
                            SkyHub::cetakTabelJadwal();
                            break;

                        case 2:
                            cout << "1. Daftarkan Paspor Baru\n2. Cari Paspor via AVL Tree\nSub-Pilihan: "; cin >> subMenu;
                            if(subMenu == 1) {
                                cout << "Masukkan Nomor Paspor  : "; cin >> paspor;
                                // [ALPRO-10] PEMBERSIHAN MEMORI BUFFER SEBELUM MEMANGGIL GETLINE
                                cin.ignore(1000, '\n'); 
                                cout << "Masukkan Nama Lengkap  : "; 
                                // [ALPRO-10] GETLINE STRATEGY UNTUK MEMBACA STRING YANG MEMILIKI KARAKTER SPASI ' '
                                getline(cin, nama); 
                                cout << "Masukkan Kode Booking PNR Tiket: "; cin >> pnr;
                                SkyHub::rootPassengerAVL = SkyHub::insertPassenger(SkyHub::rootPassengerAVL, paspor, nama, pnr);
                                SkyHub::pushUndoLog(SkyHub::undoStackMaskapai, "Mendaftarkan paspor " + paspor);
                                SkyHub::cariData(1);
                            } else {
                                cout << "Masukkan Nomor Paspor: "; cin >> paspor;
                                SkyHub::PenumpangNode* hasil = SkyHub::searchPassenger(SkyHub::rootPassengerAVL, paspor);
                                if(hasil) cout << "DATA MATCH PADA AVL NODE! Nama Penumpang: " << hasil->namaLengkap << " | Status: Siap Boarding\n";
                                else cout << "Data paspor penumpang tidak terdaftar.\n";
                            }
                            break;

                        case 3:
                            cout << "1. Input Peta Gerbang\n2. Lookup Informasi Gerbang O(1)\nSub-Pilihan: "; cin >> subMenu;
                            if(subMenu == 1) {
                                SkyHub::cetakTabelJadwal();
                                cout << "\n" << "Masukkan Kode Penerbangan: "; cin >> kode;
                                cout << "Detail Gerbang & Status  : "; cin >> detailGerbang;
                                SkyHub::informasiGerbangHashMap[kode] = detailGerbang;
                                SkyHub::pushUndoLog(SkyHub::undoStackMaskapai, "Mapping gerbang " + kode);
                            } else {
                                SkyHub::cetakTabelJadwal();
                                cout << "\n" << "Ketik Kode Penerbangan: "; cin >> kode;
                                if(SkyHub::informasiGerbangHashMap.find(kode) != SkyHub::informasiGerbangHashMap.end()) {
                                    SkyHub::cariData(SkyHub::informasiGerbangHashMap[kode]);
                                } else cout << "Info Gerbang Belum Terpetakan.\n";
                            }
                            break;
                        case 4:
                            SkyHub::cetakTabelJadwal();
                            cout << "\n" << "Masukkan Kode Pesawat untuk Dihapus: "; cin >> kode;
                            {
                                string findKota = "", findJam = "";
                                SkyHub::Penerbangan* temp = SkyHub::headPenerbangan;
                                while(temp) {
                                    if(temp->kodePenerbangan == kode) {
                                        findKota = temp->kotaTujuan;
                                        findJam = temp->jamKeberangkatan;
                                        break;
                                    }
                                    temp = temp->next;
                                }
                                if(findKota != "") {
                                    SkyHub::pushUndoLog(SkyHub::undoStackMaskapai, SkyHub::HAPUS_JADWAL, "Menghapus jadwal " + kode, kode, findKota, findJam);
                                } else {
                                    SkyHub::pushUndoLog(SkyHub::undoStackMaskapai, "Gagal menghapus jadwal " + kode);
                                }
                            }
                            SkyHub::hapusJadwalPenerbangan(kode);
                            break;
                            
                        case 5:
                            SkyHub :: cetakDataPassenggerInOrder(SkyHub::rootPassengerAVL);
                            cout << "Masukkan Nomor Paspor Penumpang untuk Dihapus: "; cin >> paspor;
                            SkyHub::rootPassengerAVL = SkyHub::deletePassenger(SkyHub::rootPassengerAVL, paspor);
                            if (SkyHub::searchPassenger(SkyHub::rootPassengerAVL, paspor) == nullptr) {
                                cout << "Paspor " << paspor << " berhasil dihapus dari sistem.\n";
                            } else {
                                cout << "Paspor " << paspor << " tidak ditemukan dalam sistem.\n";
                            }
                            SkyHub::pushUndoLog(SkyHub::undoStackMaskapai, "Menghapus paspor " + paspor);
                            break;

                        case 6:
                            SkyHub::popUndoLog(SkyHub::undoStackMaskapai);
                            break;
                    }
                } while (pilihanMenu != 0);
                continue;
            }

            if (roleAccess == "Bandara") {
                // [ALPRO-9] DASHBOARD MENARA KONTROL & BANDARA
                do {
                    cout << "\n--- DASHBOARD MENARA KONTROL & BANDARA  ---";
                    cout << "\n1. Atur Antrean Lepas Landas Runway (Priority Queue)";
                    cout << "\n2. Cek Konektivitas Jaringan Rute Udara (Graph BFS & DFS)";
                    cout << "\n3. Rotasi Shift Kerja Petugas Menara Kontrol (Circular LL)";
                    cout << "\n4. Terbangkan Pesawat (Dequeue Runway & Try-Catch)";
                    cout << "\n5. Tambah Petugas Shift (Circular LL Insert)";
                    cout << "\n6. Hapus Petugas Shift (Circular LL Delete)";
                    cout << "\n7. Batalkan Aksi Terakhir / Undo (Stack Pop)";
                    cout << "\n8. Audit Arsip Keberangkatan (Vector Sort & Callback)";
                    cout << "\n0. logout Portal";
                    cout << "\nPilihan Operasi Bandara: ";

                    // Validasi Input Menu Bandara
                    jmlhmenu = 8; // Jumlah menu bandara
                    pilihanMenu = SkyHub::dapatkanPilihanMenu(jmlhmenu);

                    switch (pilihanMenu) {
                        case 1:
                            SkyHub::cetakTabelJadwal();
                            cout << "Masukkan Kode Pesawat di Runway: "; cin >> kode;
                            cout << "Tingkat Urgensi (1-3 | 3 = Darurat Kerusakan Mesin): "; cin >> subMenu;
                            SkyHub::runwayPriorityQueue.push({kode, subMenu});
                            cout << "Pesawat aman terdaftar di antrean menara pengawas.\n";
                            SkyHub::pushUndoLog(SkyHub::undoStackBandara, "Registrasi antrean pesawat " + kode);
                            break;

                        case 2:
                            SkyHub::traverseBFS("Bandara_Soekarno_Hatta");
                            cout << "\n";
                            SkyHub::traverseDFS("Bandara_Soekarno_Hatta");
                            break;

                        case 3:
                            if (SkyHub::currentShiftPetugas) {
                                cout << "Petugas On-Duty Saat Ini: " << SkyHub::currentShiftPetugas->namaPetugas << endl;
                                SkyHub::currentShiftPetugas = SkyHub::currentShiftPetugas->next;
                                cout << "Petugas Shift Berikutnya: " << SkyHub::currentShiftPetugas->namaPetugas << endl;
                                string statusRef = "ON_DUTY";
                                SkyHub::updateStatusOperasional(statusRef); // Default argument triggers STANDBY if not passed, but here we pass ON_DUTY. Let's demonstrate default.
                                string defStatus = "GANTI_SHIFT";
                                SkyHub::updateStatusOperasional(defStatus); // Memanggil secara normal
                                SkyHub::pushUndoLog(SkyHub::undoStackBandara, "Rotasi shift kerja petugas");
                            } else {
                                cout << "Tidak ada petugas shift yang tersedia.\n";
                            }
                            break;

                        case 4:
                            try {
                                if(SkyHub::runwayPriorityQueue.empty()) {
                                    throw runtime_error("Antrean Runway kosong! Tidak ada pesawat yang bisa diterbangkan.");
                                }
                                SkyHub::RunwayItem pesawatTerbang = SkyHub::runwayPriorityQueue.top();
                                SkyHub::runwayPriorityQueue.pop();
                                cout << "[PRIORITY QUEUE DEQUEUE] Pesawat " << pesawatTerbang.kodePenerbangan 
                                     << " (Urgensi: " << pesawatTerbang.tingkatUrgensi << ") berhasil lepas landas!\n";
                                
                                string statusUrgensi = (pesawatTerbang.tingkatUrgensi == 3) ? "Darurat" : (pesawatTerbang.tingkatUrgensi == 2 ? "Prioritas" : "Reguler");
                                string logKeberangkatan = "[" + statusUrgensi + "] Pesawat " + pesawatTerbang.kodePenerbangan + " Berhasil Lepas Landas";
                                SkyHub::arsipKeberangkatan.push_back(logKeberangkatan);
                                SkyHub::pushUndoLog(SkyHub::undoStackBandara, "Penerbangan pesawat " + pesawatTerbang.kodePenerbangan);
                            } catch(const exception& e) {
                                cout << "[EXCEPTION DITANGKAP] Error: " << e.what() << "\n";
                            }
                            break;
                            
                        case 5:
                            cout << "Masukkan Nama Petugas Baru: "; cin >> nama;
                            cout << "Masukkan Divisi           : "; cin >> kode;
                            SkyHub::tambahPetugasBandara(nama, kode);
                            cout << "Petugas " << nama << " sukses ditambahkan ke sirkulasi shift.\n";
                            SkyHub::pushUndoLog(SkyHub::undoStackBandara, "Penambahan petugas baru " + nama);
                            break;

                        case 6:
                            SkyHub :: cetakPetugasShift(); 
                            cout <<"\n Masukkan Nama Petugas untuk Dihapus: "; cin >> nama;
                            SkyHub::hapusPetugas(nama);
                            SkyHub::pushUndoLog(SkyHub::undoStackBandara, "Penghapusan petugas " + nama);
                            break;

                        case 7:
                            SkyHub::popUndoLog(SkyHub::undoStackBandara);
                            break;

                        case 8: {
                            // [STRUKDAT-8] ALGORITMA SORTING & [ALPRO-11] LAMBDA EXPRESSION
                            sort(SkyHub::arsipKeberangkatan.begin(), SkyHub::arsipKeberangkatan.end(), [](const string& a, const string& b) { 
                                return a < b; // Mengurutkan nama pesawat secara Ascending (A-Z)
                            });
                            
                            cout << "\n--- Urutan Riwayat Keberangkatan (A-Z): ---\n";
                            vector<string>::iterator it;
                            for (it = SkyHub::arsipKeberangkatan.begin(); it != SkyHub::arsipKeberangkatan.end(); ++it) {
                            cout << " -> " << *it << "\n";
                            }
                            
                            string keyword;
                            cout << "\nMasukkan tipe riwayat yang ingin dihitung (Reguler/Prioritas/Darurat): "; 
                            cin >> keyword;
                            
                            // [ALPRO-11] LAMBDA menangkap (capture) keyword lokal
                            auto kriteria = [keyword](const string& log) {
                                return log.find(keyword) != string::npos;
                            };
                            
                            SkyHub::auditLogPenerbangan(kriteria, SkyHub::arsipKeberangkatan);
                            break;
                        }
                    }
                } while (pilihanMenu != 0);
                continue;
            }
        } 
        
        if (loginChoice == 2) {
            SkyHub::daftarUserBaru();
            continue;
        }
    } while (loginChoice != 0);

    // Save semua data ke file text sebelum program ditutup
    SkyHub::simpanSemuaData();

    // Mencegah Memory Leak saat exit program
    SkyHub::bersihkanMemoriGlobal();

return 0;
}