#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream> // Added for file handling
#include <cmath>

// Fungsi untuk mengonversi prefix menjadi subnet mask dalam format desimal
std::vector<int> prefix_to_subnet_mask(int prefix) {
    std::vector<int> subnet_mask(32, 0);
    for (int i = 0; i < prefix; ++i)
        subnet_mask[i] = 1;

    std::vector<int> subnet_mask_decimal;
    for (int i = 0; i < 32; i += 8) {
        int octet_decimal = 0;
        for (int j = i; j < i + 8; ++j) {
            octet_decimal <<= 1;
            octet_decimal |= subnet_mask[j];
        }
        subnet_mask_decimal.push_back(octet_decimal);
    }
    return subnet_mask_decimal;
}

// Fungsi untuk menghitung jumlah subnet, jumlah host, dan blok subnet
void calculate_subnet_parameters(const std::vector<int>& subnet_mask_decimal, int& num_subnets, int& num_hosts, int& block_size) {
    int num_ones = 0; // Menghitung jumlah bit 1 di oktet terakhir
    int octet = subnet_mask_decimal[3];
    while (octet > 0) {
        if (octet & 1)
            ++num_ones;
        octet >>= 1;
    }

    num_subnets = 1 << num_ones; // Jumlah subnet (2 pangkat x)
    num_hosts = (1 << (8 - num_ones)) - 2; // Jumlah host per subnet ((2 pangkat y) - 2)
    block_size = 256 - subnet_mask_decimal[3];
}




// Fungsi untuk menghasilkan tabel subnet
void generate_subnet_table(const std::string& ip_address, int prefix, int num_entries) {
    std::vector<int> subnet_mask_decimal = prefix_to_subnet_mask(prefix);
    int num_subnets, num_hosts, block_size;
    calculate_subnet_parameters(subnet_mask_decimal, num_subnets, num_hosts, block_size);

    std::vector<int> subnet_mask_binary;
    for (int octet : subnet_mask_decimal) {
        for (int i = 7; i >= 0; --i) {
            subnet_mask_binary.push_back((octet >> i) & 1);
        }
    }

    std::vector<int> ip_octets;
    std::stringstream ss(ip_address);
    std::string octet;
    while (std::getline(ss, octet, '.')) {
        ip_octets.push_back(std::stoi(octet));
    }

    std::ofstream outputFile("hasil.txt"); // Open the file for writing
    if (outputFile.is_open()) {
        outputFile << "Jumlah Subnet: " << num_subnets << std::endl;
        outputFile << "Jumlah Host per Subnet: " << num_hosts << std::endl;
        outputFile << "Blok Subnet: " << block_size << std::endl;
        outputFile << "Subnet Mask (Binary): ";
        for (int i = 0; i < subnet_mask_binary.size(); ++i) {
            outputFile << subnet_mask_binary[i];
            if ((i + 1) % 8 == 0) {
                outputFile << " ";
            }
        }
        outputFile << std::endl;

        outputFile << "\nTabel Subnet:\n";
        outputFile << "Subnet\t\t| IP Awal\t| IP Akhir\t| IP Broadcast\n";
        outputFile << "-----------------------------------------------\n";
        for (int i = 0; i < num_entries; ++i) {
            outputFile << ip_octets[0] << "." << ip_octets[1] << "." << ip_octets[2] << "." << ip_octets[3] << " | ";
            outputFile << ip_octets[0] << "." << ip_octets[1] << "." << ip_octets[2] << "." << ip_octets[3] + 1 << " | ";
            outputFile << ip_octets[0] << "." << ip_octets[1] << "." << ip_octets[2] << "." << ip_octets[3] + block_size - 2 << " | ";
            outputFile << ip_octets[0] << "." << ip_octets[1] << "." << ip_octets[2] << "." << ip_octets[3] + block_size - 1 << std::endl;

            ip_octets[3] += block_size;
            if (ip_octets[3] > 255) {
                ip_octets[3] = 0;
                ip_octets[2]++;
            }
            if (ip_octets[2] > 255) {
                ip_octets[2] = 0;
                ip_octets[1]++;
            }
            if (ip_octets[1] > 255) {
                ip_octets[1] = 0;
                ip_octets[0]++;
            }
        }
        outputFile.close(); // Close the file
        std::cout << "Hasil telah disimpan dalam file hasil.txt" << std::endl;
    } else {
        std::cout << "Gagal menyimpan hasil ke file" << std::endl;
    }
}

int main() {
    std::string ip_address;
    int prefix, num_entries;

    std::cout << "Masukkan IP address (format xxx.xxx.xxx.xxx): ";
    std::cin >> ip_address;
    std::cout << "Masukkan prefix: ";
    std::cin >> prefix;
    std::cout << "Masukkan jumlah baris tabel: ";
    std::cin >> num_entries;

    generate_subnet_table(ip_address, prefix, num_entries);

    return 0;
}
