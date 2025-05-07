## Структура проекта:
```
├── datasets/             <- Папка с входными данными (CSV файлами)
│   ├── it_services_dataset_diverse_100.csv
│   ├── it_services_dataset_diverse_8100.csv
│   ├── ... другие CSV файлы с данными ...
│   └── it_services_dataset_diverse_96100.csv
├── docs/                 <- Папка для сгенерированной документации
│   └── html/
│       └── index.html    <- HTML файл с документацией
│       └── ... другие файлы документации ...
├── results/
│   ├── timing_results_bvg_all.csv  <- Сгенерированный CSV с замерами времени
│   └── sorted_services_96100_std_sort.csv <- Отсортированный датасет
├── lab1.cpp              <- Основной файл с C++ кодом
├── gen.ipynb             <- Тетрадка с генерацией данных
├── Doxyfile              <- Файл конфигурации Doxygen
├── README.md             <- Описание проекта
└── viz.ipynb             <- Графики и вывод
```
