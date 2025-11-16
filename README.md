### UDP File Management System — C++ (4 Clients Model)
## Project using UDP Sockets, File Management, and Admin Authentication
 - Ky projekt implementon një sistem komunikimi Server–Klient duke përdorur UDP sockets në C++.
 - Sistemi menaxhon fajllat në mënyrë qendrore në server dhe i lejon klientëve të qasen në to sipas rolit që kanë (read-only ose admin).

## Projekti përbëhet nga 4 pajisje:
1.  Serveri (server.cpp)
    - Serveri UDP pret komandat nga klientët dhe ekzekuton funksione të ndryshme të menaxhimit të fajllave.
    - Funksionet e mbështetura:
       - a.  Pa autentikim (read-only):
         -  /list — liston të gjithë fajllat në folderin data/
         - /read <file> — lexon përmbajtjen e një fajlli
         - /search <keyword> — kërkon tekst brenda të gjithë fajllave
       - b. Me autentikim (admin):
           - /auth <sekreti> — aktivizon mënyrën admin
           - /delete <file> — fshin fajllin
           - /info <file> — tregon madhësinë dhe datën e modifikimit
           - /upload <filename>|<content> — ngarkon një fajll të ri
           - /download <filename> — shkarkon fajllin
           - /list / /read / /search — po ashtu të lejuara
  2. Klienti Admin (clientAdmin.cpp)
       - Ky klient ka akses të plotë në sistem, pas autentikimit:
       - Në fillim shkruan: /auth letmein
 - Pastaj mund të përdorë:
      /list
      /read <file>
      /delete <file>
      /search <word>
      /info <file>
      /upload <path-lokal>

  3. Klienti Read-Only (clientReadOnly.cpp)
      - Ky klient është më i kufizuar. Nuk ka akses admin dhe nuk përdor /auth.
      - Komandat e lejuara janë:
        /list
        /read <file>
        /search <keyword>
      - Nuk mund të fshijë, modifikojë ose ngarkojë fajlla.

  4. File Shared (shared.cpp / shared.h)
 - Përmban logjikën qendrore të: menaxhimit të fajllave, leximit/shkrimit, kërkimit, informacioneve për fajllat, trajtimit të komandave që vijnë nga klientët.

##  Funksionaliteti Teknik
 -  Protokolli:
     - Projekti funksionon me protokollin UDP:
     - Pa lidhje (connectionless),
     - Shpejtësi më e madhe,
     - Nuk garanton dorëzimin e mesazhit (por e kompensojmë me logjikë kontrolli).
 -  Buffer
     - BUFFER_SIZE = 4096
 -  Porti i Përdorur
     - PORT = 12345
 -  Autentikimi
     - Çelësi i fshehtë admin:
     - ADMIN_SECRET = "letmein"

## Struktura e projektit
UDP_Project/

│

├── server.cpp

├── clientAdmin.cpp

├── clientReadOnly.cpp

├── shared.cpp

├── shared.h

│

├── data/            ← krijohet automatikisht

├── Downloads/       ← ku ruhen fajllat që shkarkohen

│

└── README.md

## Si të ekzekutohet projekti
1.  Starto serverin
 - Run server.exe
2.  Starto klientët
 - Admin: Run clientAdmin.exe
 - Read-only: Run clientReadOnly.exe
3.  Në klientin admin jep komandën: /auth letmein

## Autore: 
 - Elsa Krasniqi
 - Elona Kuqi
 - Fahrije Gjokiqi
 - Florjete Kuka

 - Universiteti i Prishtines / Lënda: Rrjeta Kompjuterike
