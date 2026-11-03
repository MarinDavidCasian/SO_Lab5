#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

bool is_prime(int n) {
    if (n < 2) return false;
    for (int d = 2; d * d <= n; ++d) {
        if (n % d == 0) return false;
    }
    return true;
}

int main() {
    const int NUM_PROCS = 10;
    const int STEP = 1000;

    int p2c[NUM_PROCS][2];
    int c2p[NUM_PROCS][2];
    pid_t pids[NUM_PROCS];

    for (int i = 0; i < NUM_PROCS; ++i) {
        if (pipe(p2c[i]) < 0 || pipe(c2p[i]) < 0) {
            std::cerr << "Eroare la crearea pipe-ului!\n";
            return 1;
        }

        pids[i] = fork();
        if (pids[i] < 0) {
            std::cerr << "Eroare la crearea procesului copil!\n";
            return 1;
        }

        if (pids[i] == 0) {
            close(p2c[i][1]);
            close(c2p[i][0]);

            int range[2];
            if (read(p2c[i][0], range, sizeof(range)) > 0) {
                int start = range[0];
                int end = range[1];

                for (int val = start; val <= end; ++val) {
                    if (is_prime(val)) {
                        write(c2p[i][1], &val, sizeof(int));
                    }
                }
            }

            close(p2c[i][0]);
            close(c2p[i][1]);
            _exit(0);
        } else {
            close(p2c[i][0]);
            close(c2p[i][1]);

            int range[2] = { i * STEP + 1, (i + 1) * STEP };
            write(p2c[i][1], range, sizeof(range));
            close(p2c[i][1]);
        }
    }

    std::cout << "Numere prime pana la 10000:\n";
    for (int i = 0; i < NUM_PROCS; ++i) {
        int prime;
        while (read(c2p[i][0], &prime, sizeof(int)) > 0) {
            std::cout << prime << " ";
        }
        close(c2p[i][0]);
    }
    std::cout << "\n";

    for (int i = 0; i < NUM_PROCS; ++i) {
        waitpid(pids[i], nullptr, 0);
    }

    return 0;
}#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

bool is_prime(int n) {
    if (n < 2) return false;
    for (int d = 2; d * d <= n; ++d) {
        if (n % d == 0) return false;
    }
    return true;
}

int main() {
    const int NUM_PROCS = 10;
    const int STEP = 1000;

    int p2c[NUM_PROCS][2];
    int c2p[NUM_PROCS][2];
    pid_t pids[NUM_PROCS];

    for (int i = 0; i < NUM_PROCS; ++i) {
        if (pipe(p2c[i]) < 0 || pipe(c2p[i]) < 0) {
            std::cerr << "Eroare la crearea pipe-ului!\n";
            return 1;
        }

        pids[i] = fork();
        if (pids[i] < 0) {
            std::cerr << "Eroare la crearea procesului copil!\n";
            return 1;
        }

        if (pids[i] == 0) {
            close(p2c[i][1]);
            close(c2p[i][0]);

            int range[2];
            if (read(p2c[i][0], range, sizeof(range)) > 0) {
                int start = range[0];
                int end = range[1];

                for (int val = start; val <= end; ++val) {
                    if (is_prime(val)) {
                        write(c2p[i][1], &val, sizeof(int));
                    }
                }
            }

            close(p2c[i][0]);
            close(c2p[i][1]);
            _exit(0);
        } else {
            close(p2c[i][0]);
            close(c2p[i][1]);

            int range[2] = { i * STEP + 1, (i + 1) * STEP };
            write(p2c[i][1], range, sizeof(range));
            close(p2c[i][1]);
        }
    }

    std::cout << "Numere prime pana la 10000:\n";
    for (int i = 0; i < NUM_PROCS; ++i) {
        int prime;
        while (read(c2p[i][0], &prime, sizeof(int)) > 0) {
            std::cout << prime << " ";
        }
        close(c2p[i][0]);
    }
    std::cout << "\n";

    for (int i = 0; i < NUM_PROCS; ++i) {
        waitpid(pids[i], nullptr, 0);
    }

    return 0;
}
