#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <chrono>
#include <algorithm>
#include <stdexcept>
#include <iomanip>    // Для std::fixed, std::setprecision
#include <utility>    // Для std::move
#include <locale.h>
#include <windows.h>


/**
 * @brief Представляет IT-услугу с ее свойствами.
 *
 * Хранит название, ориентировочную стоимость, срок исполнения (в днях)
 * и размер предоплаты для IT-услуги.
 * Включает перегруженные операторы сравнения на основе стоимости, предоплаты и названия.
 */
struct Service {
    std::string name;       ///< Название услуги
    double cost;            ///< Ориентировочная стоимость
    int duration;           ///< Срок исполнения (дни)
    double prepayment;      ///< Размер предоплаты

    /**
     * @brief Конструктор по умолчанию.
     */
    Service() : cost(0.0), duration(0), prepayment(0.0) {}

    /**
     * @brief Параметризованный конструктор.
     * @param n Название услуги.
     * @param c Ориентировочная стоимость.
     * @param d Срок исполнения в днях.
     * @param p Размер предоплаты.
     */
    Service(std::string n, double c, int d, double p)
        : name(std::move(n)), cost(c), duration(d), prepayment(p) {}

    /**
     * @brief Оператор "меньше" (<).
     * Сравнивает услуги сначала по стоимости, затем по предоплате, затем по названию.
     * @param other Услуга, с которой производится сравнение.
     * @return True, если текущая услуга "меньше" другой, иначе false.
     */
    bool operator<(const Service& other) const {
        if (cost != other.cost) {
            return cost < other.cost;
        }
        if (prepayment != other.prepayment) {
            return prepayment < other.prepayment;
        }
        return name < other.name;
    }

    /**
     * @brief Оператор "больше" (>).
     * @param other Услуга, с которой производится сравнение.
     * @return True, если текущая услуга "больше" другой, иначе false.
     */
    bool operator>(const Service& other) const {
        return other < *this;
    }

    /**
     * @brief Оператор "меньше или равно" (<=).
     * @param other Услуга, с которой производится сравнение.
     * @return True, если текущая услуга "меньше или равна" другой, иначе false.
     */
    bool operator<=(const Service& other) const {
        return !(other < *this);
    }

    /**
     * @brief Оператор "больше или равно" (>=).
     * @param other Услуга, с которой производится сравнение.
     * @return True, если текущая услуга "больше или равна" другой, иначе false.
     */
    bool operator>=(const Service& other) const {
        return !(*this < other);
    }

     /**
      * @brief Оператор равенства (==). Может быть полезен для некоторых алгоритмов или проверок.
      * Проверяет равенство всех полей, используемых для сортировки (стоимость, предоплата, название).
      * @param other Услуга, с которой производится сравнение.
      * @return True, если услуги считаются равными по критериям сортировки.
      */
    bool operator==(const Service& other) const {
        return cost == other.cost && prepayment == other.prepayment && name == other.name;
    }

     /**
      * @brief Оператор неравенства (!=).
      * @param other Услуга, с которой производится сравнение.
      * @return True, если услуги не равны.
      */
    bool operator!=(const Service& other) const {
        return !(*this == other);
    }
};


/**
 * @brief Перегружает оператор ввода (>>) для чтения объекта Service из потока.
 * Ожидает формат CSV: Название,Стоимость,Срок,Предоплата
 * @param is Входной поток.
 * @param s Объект Service для заполнения.
 * @return Входной поток.
 */
std::istream& operator>>(std::istream& is, Service& s) {
    std::string line;
    if (std::getline(is, line)) {
        std::stringstream ss(line);
        std::string segment;

        if (!std::getline(ss, s.name, ',')) return is;
        if (!std::getline(ss, segment, ',')) return is;
        try { s.cost = std::stod(segment); } catch (...) { s.cost = 0.0; }
        if (!std::getline(ss, segment, ',')) return is;
        try { s.duration = std::stoi(segment); } catch (...) { s.duration = 0; }
        if (!std::getline(ss, segment)) return is;
        try { s.prepayment = std::stod(segment); } catch (...) { s.prepayment = 0.0; }
    } else {
        is.setstate(std::ios::eofbit);
    }
    return is;
}


/**
 * @brief Перегружает оператор вывода (<<) для записи объекта Service в поток.
 * Выводит в формате CSV: Название,Стоимость,Срок,Предоплата
 * @param os Выходной поток.
 * @param s Объект Service для записи.
 * @return Выходной поток.
 */
std::ostream& operator<<(std::ostream& os, const Service& s) {
    os << s.name << ","
       << std::fixed << std::setprecision(2) << s.cost << ","
       << s.duration << ","
       << std::fixed << std::setprecision(2) << s.prepayment;
    return os;
}

/**
 * @brief Загружает данные об услугах из CSV-файла.
 * @param filename Путь к CSV-файлу.
 * @param services Вектор для сохранения загруженных объектов Service (будет очищен перед загрузкой).
 * @return True, если загрузка прошла успешно (прочитан хотя бы заголовок и есть данные), иначе false.
 * @throws std::runtime_error Если файл не удается открыть.
 */
bool loadServices(const std::string& filename, std::vector<Service>& services) {
    services.clear();
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        throw std::runtime_error("Ошибка: Не удалось открыть входной файл: " + filename);
    }

    std::string header;
    if (!std::getline(inFile, header)) {
        std::cerr << "Предупреждение: Не удалось прочитать заголовок или файл пуст: " << filename << std::endl;
        inFile.close();
        return false;
    }

    Service tempService;
    while (inFile >> tempService) {
       services.push_back(tempService);
    }

    if (inFile.bad()) {
        std::cerr << "Ошибка чтения данных из файла: " << filename << std::endl;
        inFile.close();
        return false;
    }

    inFile.close();
     if (services.empty()) {
         std::cerr << "Предупреждение: Файл '" << filename << "' содержит только заголовок или данные некорректны." << std::endl;
         return false;
     }

    return true;
}


/**
 * @brief Сохраняет данные об услугах в CSV-файл.
 * @param filename Путь к выходному CSV-файлу.
 * @param services Вектор, содержащий объекты Service для сохранения.
 * @return True, если сохранение прошло успешно, иначе false.
 * @throws std::runtime_error Если файл не удается открыть для записи.
 */
bool saveServices(const std::string& filename, const std::vector<Service>& services) {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        throw std::runtime_error("Ошибка: Не удалось открыть выходной файл для записи: " + filename);
    }

    outFile << "Название услуги,Ориентировочная стоимость,Срок исполнения (дни),Размер предоплаты\n";

    for (const auto& service : services) {
        outFile << service << "\n";
    }

    if (outFile.bad()) {
        std::cerr << "Ошибка записи в файл: " << filename << std::endl;
        outFile.close();
        return false;
    }

    outFile.close();
    return true;
}


/**
 * @brief Сортирует вектор объектов Service методом пузырька.
 * @param arr Вектор Service для сортировки (изменяется на месте).
 */
void bubbleSort(std::vector<Service>& arr) {
    int n = arr.size();
    for (int i = 0; i < n - 1; ++i) {
        for (int j = 0; j < n - i - 1; ++j) {
            if (arr[j] > arr[j + 1]) {
                std::swap(arr[j], arr[j + 1]);
            }
        }
    }
}


/**
 * @brief Сортирует вектор объектов Service методом вставок.
 * @param arr Вектор Service для сортировки (изменяется на месте).
 */
void insertionSort(std::vector<Service>& arr) {
    int n = arr.size();
    for (int i = 1; i < n; ++i) {
        Service key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
    }
}


/**
 * @brief Сортирует вектор объектов Service методом Шейкера (двунаправленная пузырьковая).
 * @param arr Вектор Service для сортировки (изменяется на месте).
 */
void shakerSort(std::vector<Service>& arr) {
    int n = arr.size();
    bool swapped = true;
    int start = 0;
    int end = n - 1;

    while (swapped) {
        swapped = false;
        for (int i = start; i < end; ++i) {
            if (arr[i] > arr[i + 1]) {
                std::swap(arr[i], arr[i + 1]);
                swapped = true;
            }
        }
        if (!swapped) break;

        swapped = false;
        --end;

        for (int i = end - 1; i >= start; --i) {
            if (arr[i] > arr[i + 1]) {
                std::swap(arr[i], arr[i + 1]);
                swapped = true;
            }
        }
        ++start;
    }
}


/**
 * @brief Измеряет время выполнения заданной функции сортировки.
 * @tparam SortFunc Тип функции сортировки (например, void(*)(std::vector<Service>&)).
 * @param sortFunction Функция сортировки для измерения времени.
 * @param data Вектор объектов Service для сортировки (будет скопирован).
 * @param algorithmName Название алгоритма для вывода.
 * @return Продолжительность сортировки в миллисекундах (double).
 */
template<typename SortFunc>
double timeSort(SortFunc sortFunction, const std::vector<Service>& data, const std::string& algorithmName) {
    std::vector<Service> dataCopy = data;
    auto start = std::chrono::high_resolution_clock::now();
    sortFunction(dataCopy);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration_ms = end - start;
    return duration_ms.count();
}


/**
 * @brief Главная функция программы.
 * Загружает данные разного размера из файлов, проводит эксперименты по сортировке
 * для всех заданных алгоритмов и сохраняет результаты замеров времени.
 */
int main() {
    SetConsoleOutputCP(CP_UTF8);
    //setlocale(LC_ALL, "Russian");

    const std::vector<int> datasetSizes = {
        100, 8100, 16100, 24100, 32100, 40100, 48100,
        56100, 64100, 72100, 80100, 88100, 96100
    };

    const std::string DATASETS_DIR = "datasets/";
    const std::string FILENAME_PATTERN = "it_services_dataset_diverse_";

    const std::string OUTPUT_FILENAME_BASE = "results/sorted_services";
    const std::string TIMING_RESULTS_FILENAME = "results/timing_results_bvg_all.csv";

    std::ofstream timingFile(TIMING_RESULTS_FILENAME, std::ios::binary);
    if (!timingFile.is_open()) {
        std::cerr << "Ошибка: Не удалось открыть файл для записи результатов замеров: " << TIMING_RESULTS_FILENAME << std::endl;
        return 1;
    }

    timingFile << "DatasetSize,Algorithm,TimeMilliseconds\n";
    std::cout << "Файл для сохранения результатов замеров времени '" << TIMING_RESULTS_FILENAME << "' успешно открыт." << std::endl;

    std::vector<Service> currentData;

    for (int currentSize_int : datasetSizes) {
        size_t currentSize = static_cast<size_t>(currentSize_int);
        std::string filename = DATASETS_DIR + FILENAME_PATTERN + std::to_string(currentSize) + ".csv";

        std::cout << "\n--- Обработка файла: " << filename << " (размер: " << currentSize << ") ---" << std::endl;

        try {
            if (!loadServices(filename, currentData)) {
                std::cerr << "Пропуск экспериментов для размера " << currentSize << " из-за ошибки загрузки или пустого файла." << std::endl;
                continue;
            }
             if (currentData.size() != currentSize) {
                 std::cerr << "Предупреждение: Ожидалось " << currentSize << " записей в файле, но загружено " << currentData.size() << "." << std::endl;
                 currentSize = currentData.size();
             }
            std::cout << "Загружено " << currentData.size() << " записей." << std::endl;
        } catch (const std::runtime_error& e) {
            std::cerr << e.what() << std::endl;
            std::cerr << "Пропуск экспериментов для размера " << currentSize << " из-за ошибки открытия файла." << std::endl;
            continue;
        }

        double bubbleTime = timeSort(bubbleSort, currentData, "Сортировка пузырьком");
        std::cout << "Сортировка пузырьком завершена за " << std::fixed << std::setprecision(4) << bubbleTime << " мс." << std::endl;
        timingFile << currentSize << "," << "\"Сортировка пузырьком\"" << "," << std::fixed << std::setprecision(4) << bubbleTime << "\n";

        double insertionTime = timeSort(insertionSort, currentData, "Сортировка вставками");
        std::cout << "Сортировка вставками завершена за " << std::fixed << std::setprecision(4) << insertionTime << " мс." << std::endl;
        timingFile << currentSize << "," << "\"Сортировка вставками\"" << "," << std::fixed << std::setprecision(4) << insertionTime << "\n";

        double shakerTime = timeSort(shakerSort, currentData, "Шейкер-сортировка");
        std::cout << "Шейкер-сортировка завершена за " << std::fixed << std::setprecision(4) << shakerTime << " мс." << std::endl;
        timingFile << currentSize << "," << "\"Шейкер-сортировка\"" << "," << std::fixed << std::setprecision(4) << shakerTime << "\n";

        double stdSortTime = timeSort( [](std::vector<Service>& vec){ std::sort(vec.begin(), vec.end()); }, currentData, "std::sort");
        std::cout << "std::sort завершена за " << std::fixed << std::setprecision(4) << stdSortTime << " мс." << std::endl;
        timingFile << currentSize << "," << "\"std::sort\"" << "," << std::fixed << std::setprecision(4) << stdSortTime << "\n";

        timingFile.flush();
    }

     if (!currentData.empty()) {
        std::cout << "\nСохранение отсортированного результата (std::sort) для самого большого обработанного набора данных (" << currentData.size() << " записей)..." << std::endl;
        std::string outputFilename = OUTPUT_FILENAME_BASE + "_" + std::to_string(currentData.size()) + "_std_sort.csv";
        try {
            std::vector<Service> finalSortedData = currentData;
            std::sort(finalSortedData.begin(), finalSortedData.end());
            if (saveServices(outputFilename, finalSortedData)) {
                std::cout << "Отсортированные данные сохранены в " << outputFilename << std::endl;
            } else {
                std::cerr << "Не удалось сохранить отсортированные данные в " << outputFilename << std::endl;
            }
        } catch (const std::runtime_error& e) {
            std::cerr << "Ошибка при сохранении отсортированных данных: " << e.what() << std::endl;
        }
    } else {
        std::cerr << "\nНет данных для сохранения финального отсортированного файла, так как ни один набор данных не был успешно загружен." << std::endl;
    }

    timingFile.close();
    std::cout << "\nФайл с результатами замеров времени '" << TIMING_RESULTS_FILENAME << "' закрыт." << std::endl;

    return 0;
}
