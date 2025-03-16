import javax.swing.*;
import javax.swing.table.DefaultTableModel;
import javax.swing.table.TableRowSorter;
import java.awt.*;
import java.awt.event.*;
import java.io.*;
import java.util.Stack;
import java.util.Vector;


/**
 * Класс {@code BookList} представляет графическое приложение для управления списком команд, гонщиков, трасс и соревнований.
 * Включает функциональность аутентификации, управления пользователями, добавления, удаления, поиска и сохранения данных.
 */
public class BookList {
    private JFrame bookList;
    private DefaultTableModel modelTeams;
    private DefaultTableModel modelDrivers;
    private DefaultTableModel modelTracks;
    private DefaultTableModel modelCompetitions;

    private final Stack<DefaultTableModel> teamsUndoStack = new Stack<>();
    private final Stack<DefaultTableModel> driversUndoStack = new Stack<>();
    private final Stack<DefaultTableModel> tracksUndoStack = new Stack<>();
    private final Stack<DefaultTableModel> competitionsUndoStack = new Stack<>();

    private JButton save;
    private JButton addFile;
    private JButton delRow;
    private JButton trash;
    private JButton addRowButton;
    private JButton manageUsers;

    private JScrollPane scrollTeams;
    private JScrollPane scrollDrivers;
    private JScrollPane scrollTracks;
    private JScrollPane scrollCompetitions;

    private JTable tableTeams;
    private JTable tableDrivers;
    private JTable tableTracks;
    private JTable tableCompetitions;

    private JTextField searchField;
    private JButton filter;
    private JButton resetButton;

    private JTabbedPane navBar;

    /**
     * Отображает главное окно приложения, выполняет инициализацию компонентов и проверяет аутентификацию пользователя.
     */
    public void show() {
        // Проверка аутентификации пользователя перед запуском приложения
        if (!checkUserAuthentication()) {
            System.exit(0); // Закрыть приложение, если регистрация обязательна
        }

        initMainFrame();
        initToolBar();
        initTablesAndTabs();
        initSearchPanel();
        initListeners();
        loadInitialData();

        bookList.setVisible(true);
    }

    /**
     * Инициализирует главное окно приложения с основными параметрами.
     */
    private void initMainFrame() {
        bookList = new JFrame("Book List");
        bookList.setSize(800, 500);
        bookList.setMinimumSize(new Dimension(800, 500));
        bookList.setLocation(250, 80);
        bookList.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        bookList.setLayout(new BorderLayout());
        ImageIcon icon = new ImageIcon(getClass().getResource("/img/icon.png"));
        bookList.setIconImage(icon.getImage());
    }

    /**
     * Инициализирует панель инструментов и добавляет кнопки управления данными.
     */
    private void initToolBar() {
        save = createIconButton("/img/save.png", "Сохранить изменения из вкладки");
        addFile = createIconButton("/img/plus.png", "Прочесть файл во вкладку");
        addRowButton = createIconButton("/img/addRow.png", "Добавить новую строку в текущую вкладку");
        delRow = createIconButton("/img/minus.png", "Удалить строку из вкладки");
        trash = createIconButton("/img/Trash.png", "Удаление всей вкладки");
        manageUsers = createIconButton("/img/manageUsers.png", "Управление пользователями");

        JToolBar toolBar = new JToolBar(JToolBar.VERTICAL);
        toolBar.setLayout(new BoxLayout(toolBar, BoxLayout.Y_AXIS));
        toolBar.add(save);
        toolBar.add(addFile);
        toolBar.add(addRowButton);
        toolBar.add(delRow);
        toolBar.add(trash);
        toolBar.add(manageUsers);

        // Добавляем панель инструментов в главное окно
        bookList.add(toolBar, BorderLayout.WEST);

        // Обработчик для управления пользователями
        manageUsers.addActionListener(event -> showUserManagementDialog());
    }

    /**
     * Инициализирует таблицы и вкладки для отображения данных.
     */
    private void initTablesAndTabs() {
        // Таблицы и их модели
        String[] columnsTeams = {"Команда", "Страна"};
        modelTeams = new DefaultTableModel(new String[][]{}, columnsTeams);
        tableTeams = new JTable(modelTeams);
        scrollTeams = new JScrollPane(tableTeams);

        String[] columnsDrivers = {"Гонщик", "Команда", "Очки"};
        modelDrivers = new DefaultTableModel(new String[][]{}, columnsDrivers);
        tableDrivers = new JTable(modelDrivers);
        scrollDrivers = new JScrollPane(tableDrivers);

        String[] columnsTracks = {"Трасса", "Местоположение"};
        modelTracks = new DefaultTableModel(new String[][]{}, columnsTracks);
        tableTracks = new JTable(modelTracks);
        scrollTracks = new JScrollPane(tableTracks);

        String[] columnsCompetitions = {"Трасса", "Дата проведения", "Начало заезда"};
        modelCompetitions = new DefaultTableModel(new String[][]{}, columnsCompetitions);
        tableCompetitions = new JTable(modelCompetitions);
        scrollCompetitions = new JScrollPane(tableCompetitions);

        navBar = new JTabbedPane();
        navBar.addTab("Команды", scrollTeams);
        navBar.addTab("Гонщики", scrollDrivers);
        navBar.addTab("Трассы", scrollTracks);
        navBar.addTab("Соревнования", scrollCompetitions);

        bookList.add(navBar, BorderLayout.CENTER);
    }

    /**
     * Инициализирует панель поиска для фильтрации данных в таблицах.
     */
    private void initSearchPanel() {
        searchField = new JTextField("Поиск...", 30);
        filter = new JButton("Поиск");
        resetButton = new JButton("Сбросить");

        JPanel searchPanel = new JPanel();
        searchPanel.add(searchField);
        searchPanel.add(filter);
        searchPanel.add(resetButton);

        bookList.add(searchPanel, BorderLayout.SOUTH);
    }

    /**
     * Добавляет обработчики событий для элементов интерфейса.
     */
    private void initListeners() {
        // Слушатель для переключения вкладок (сброс поиска)
        navBar.addChangeListener(e -> resetSearch());

        // Добавление новой строки
        addRowButton.addActionListener(event -> {
            saveCurrentState();
            addNewRow();
        });

        // Сброс поиска
        resetButton.addActionListener(e -> {
            searchField.setText("Поиск...");
            resetSearch();
        });

        // Поиск по нажатию кнопки
        filter.addActionListener(e -> performSearch());

        // Очистка поля поиска при фокусе
        searchField.addFocusListener(new FocusAdapter() {
            public void focusGained(FocusEvent e) {
                if (searchField.getText().equals("Поиск...")) {
                    searchField.setText("");
                }
            }

            public void focusLost(FocusEvent e) {
                if (searchField.getText().isEmpty()) {
                    searchField.setText("Поиск...");
                }
            }
        });

        // Поиск при нажатии Enter
        searchField.addKeyListener(new KeyAdapter() {
            public void keyPressed(KeyEvent e) {
                if (e.getKeyCode() == KeyEvent.VK_ENTER) {
                    performSearch();
                }
            }
        });

        // Добавление данных из файла
        addFile.addActionListener(arg0 -> {
            FileDialog addFileDialog = new FileDialog(bookList, "Добавление данных", FileDialog.LOAD);
            addFileDialog.setVisible(true);

            String directory = addFileDialog.getDirectory();
            String selectedFile = addFileDialog.getFile();

            if (directory == null || selectedFile == null) {
                return;
            }

            saveCurrentState();
            String fileName = directory + selectedFile;
            DefaultTableModel selectedModel = getSelectedModel();

            try (BufferedReader reader = new BufferedReader(new FileReader(fileName))) {
                String line;
                while ((line = reader.readLine()) != null) {
                    String[] fields = line.split(",");
                    selectedModel.addRow(fields);
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        });

        // Удаление всей вкладки
        trash.addActionListener(event -> {
            saveCurrentState();
            DefaultTableModel selectedModel = getSelectedModel();

            if (selectedModel != null) {
                selectedModel.setRowCount(0);
            }
        });

        // Сохранение файла
        save.addActionListener(arg0 -> saveFile());

        // Удаление строки
        delRow.addActionListener(event -> {
            JTable selectedTable = getSelectedTable();
            DefaultTableModel selectedModel = getSelectedModel();

            int selectedRow = selectedTable.getSelectedRow();
            if (selectedRow != -1) {
                saveCurrentState();
                selectedModel.removeRow(selectedRow);
            } else {
                JOptionPane.showMessageDialog(bookList, "Выберите строку для удаления.");
            }
        });

        // Undo (Ctrl+Z)
        bookList.getRootPane().registerKeyboardAction(
                e -> undoLastAction(),
                KeyStroke.getKeyStroke(KeyEvent.VK_Z, InputEvent.CTRL_DOWN_MASK),
                JComponent.WHEN_IN_FOCUSED_WINDOW
        );

        enableSorting();
    }

    /**
     * Загружает начальные данные в таблицы из файлов.
     */
    private void loadInitialData() {
        loadData("src/csv/teams.csv", modelTeams);
        loadData("src/csv/drivers.csv", modelDrivers);
        loadData("src/csv/tracks.csv", modelTracks);
        loadData("src/csv/competitions.csv", modelCompetitions);
    }

    /**
     * Создаёт кнопку с иконкой и подсказкой.
     * @param resourcePath Путь к изображению иконки.
     * @param toolTip Текст подсказки для кнопки.
     * @return Кнопка с иконкой.
     */
    private JButton createIconButton(String resourcePath, String toolTip) {
        JButton button = new JButton(new ImageIcon(getClass().getResource(resourcePath)));
        button.setPreferredSize(new Dimension(40, 30));
        button.setToolTipText(toolTip);
        return button;
    }

    /**
     * Сохраняет текущие данные из выбранной вкладки в файл.
     */
    private void saveFile() {
        int selectedTab = navBar.getSelectedIndex();

        DefaultTableModel selectedModel;
        String fileName;
        switch (selectedTab) {
            case 0:
                selectedModel = modelTeams;
                fileName = "teams.csv";
                break;
            case 1:
                selectedModel = modelDrivers;
                fileName = "drivers.csv";
                break;
            case 2:
                selectedModel = modelTracks;
                fileName = "tracks.csv";
                break;
            case 3:
                selectedModel = modelCompetitions;
                fileName = "competitions.csv";
                break;
            default:
                return;
        }

        FileDialog saveDialog = new FileDialog(bookList, "Сохранение данных", FileDialog.SAVE);
        saveDialog.setFile(fileName); // Используем имя файла на основе текущей вкладки
        saveDialog.setVisible(true);

        String directory = saveDialog.getDirectory();
        String selectedFile = saveDialog.getFile();

        if (directory == null || selectedFile == null) {
            return; // Если пользователь нажал «отмена»
        }

        fileName = directory + selectedFile;

        // Сохранение данных в выбранный файл
        try (BufferedWriter writer = new BufferedWriter(new FileWriter(fileName))) {
            for (int i = 0; i < selectedModel.getRowCount(); i++) {
                for (int j = 0; j < selectedModel.getColumnCount(); j++) {
                    writer.write((String) selectedModel.getValueAt(i, j)); // Запись значения ячейки
                    if (j < selectedModel.getColumnCount() - 1) {
                        writer.write(","); // Разделитель
                    }
                }
                writer.write("\n"); // Новая строка
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }


    /**
     * Загружает данные из файла в указанную модель таблицы.
     * @param filePath Путь к файлу данных.
     * @param model Модель таблицы для загрузки данных.
     */
    private void loadData(String filePath, DefaultTableModel model) {
        model.setRowCount(0);
        try (BufferedReader reader = new BufferedReader(new FileReader(filePath))) {
            String line;
            while ((line = reader.readLine()) != null) {
                String[] fields = line.split(",");
                model.addRow(fields);
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    /**
     * Сохраняет текущее состояние таблицы для выполнения функции отмены действий.
     */
    private void saveCurrentState() {
        int selectedTab = navBar.getSelectedIndex();
        DefaultTableModel currentModel = getSelectedModel();

        if (currentModel != null) {
            Stack<DefaultTableModel> undoStack = getUndoStack(selectedTab);
            undoStack.push(cloneModel(currentModel));
        }
    }

    /**
     * Возвращает модель данных для выбранной вкладки.
     * @return Модель данных выбранной таблицы.
     */
    private DefaultTableModel getSelectedModel() {
        int selectedTab = navBar.getSelectedIndex();
        switch (selectedTab) {
            case 0: return modelTeams;
            case 1: return modelDrivers;
            case 2: return modelTracks;
            case 3: return modelCompetitions;
            default: return null;
        }
    }

    /**
     * Возвращает таблицу для выбранной вкладки.
     * @return Таблица из активной вкладки.
     */
    private JTable getSelectedTable() {
        int selectedTab = navBar.getSelectedIndex();
        switch (selectedTab) {
            case 0: return tableTeams;
            case 1: return tableDrivers;
            case 2: return tableTracks;
            case 3: return tableCompetitions;
            default: return null;
        }
    }

    /**
     * Возвращает стек отмены для выбранной вкладки.
     * @param selectedTab Индекс активной вкладки.
     * @return Стек отмены для выбранной таблицы.
     */
    private Stack<DefaultTableModel> getUndoStack(int selectedTab) {
        switch (selectedTab) {
            case 0: return teamsUndoStack;
            case 1: return driversUndoStack;
            case 2: return tracksUndoStack;
            case 3: return competitionsUndoStack;
            default: return null;
        }
    }

    /**
     * Клонирует текущую модель таблицы.
     * @param model Исходная модель таблицы.
     * @return Копия модели таблицы.
     */
    private DefaultTableModel cloneModel(DefaultTableModel model) {
        DefaultTableModel clone = new DefaultTableModel();

        for (int i = 0; i < model.getColumnCount(); i++) {
            clone.addColumn(model.getColumnName(i));
        }

        for (int i = 0; i < model.getRowCount(); i++) {
            Vector<Object> row = new Vector<>();
            for (int j = 0; j < model.getColumnCount(); j++) {
                row.add(model.getValueAt(i, j));
            }
            clone.addRow(row);
        }

        return clone;
    }

    /**
     * Выполняет отмену последнего действия в выбранной таблице.
     */
    private void undoLastAction() {
        int selectedTab = navBar.getSelectedIndex();
        Stack<DefaultTableModel> undoStack = getUndoStack(selectedTab);
        DefaultTableModel currentModel = getSelectedModel();

        if (!undoStack.isEmpty() && currentModel != null) {
            DefaultTableModel previousState = undoStack.pop();
            currentModel.setDataVector(previousState.getDataVector(), getColumnIdentifiers(currentModel));
        }
    }

    private Vector<String> getColumnIdentifiers(DefaultTableModel model) {
        Vector<String> columnIdentifiers = new Vector<>();
        for (int i = 0; i < model.getColumnCount(); i++) {
            columnIdentifiers.add(model.getColumnName(i));
        }
        return columnIdentifiers;
    }

    /**
     * Добавляет новую строку в текущую таблицу через диалоговое окно ввода данных.
     */
    private void addNewRow() {
        int selectedTab = navBar.getSelectedIndex(); // Получаем индекс активной вкладки
        DefaultTableModel selectedModel;
        String[] columnNames;
        String title;

        switch (selectedTab) {
            case 0:
                selectedModel = modelTeams;
                columnNames = new String[]{"Команда", "Страна"};
                title = "Добавление новой команды";
                break;
            case 1:
                selectedModel = modelDrivers;
                columnNames = new String[]{"Гонщик", "Команда", "Очки"};
                title = "Добавление нового гонщика";
                break;
            case 2:
                selectedModel = modelTracks;
                columnNames = new String[]{"Трасса", "Местоположение"};
                title = "Добавление новой трассы";
                break;
            case 3:
                selectedModel = modelCompetitions;
                columnNames = new String[]{"Трасса", "Дата проведения", "Начало заезда"};
                title = "Добавление нового соревнования";
                break;
            default:
                return;
        }

        // Окно для ввода данных
        JDialog addDialog = new JDialog(bookList, title, true);
        addDialog.setSize(400, 200);
        addDialog.setLayout(new GridLayout(columnNames.length + 1, 2));

        JTextField[] fields = new JTextField[columnNames.length];

        for (int i = 0; i < columnNames.length; i++) {
            addDialog.add(new JLabel(columnNames[i] + ":"));
            fields[i] = new JTextField();
            addDialog.add(fields[i]);
        }

        JButton addButton = new JButton("Добавить");
        JButton cancelButton = new JButton("Отмена");

        addDialog.add(addButton);
        addDialog.add(cancelButton);

        // Обработчик кнопки "Добавить"
        addButton.addActionListener(e -> {
            String[] newRow = new String[columnNames.length];
            for (int i = 0; i < columnNames.length; i++) {
                newRow[i] = fields[i].getText().trim();
            }

            // Проверка на пустые поля
            for (String field : newRow) {
                if (field.isEmpty()) {
                    JOptionPane.showMessageDialog(addDialog, "Все поля должны быть заполнены.", "Ошибка", JOptionPane.ERROR_MESSAGE);
                    return;
                }
            }

            selectedModel.addRow(newRow); // Добавляем новую строку в модель таблицы
            addDialog.dispose(); // Закрываем диалоговое окно
        });

        // Обработчик кнопки "Отмена"
        cancelButton.addActionListener(e -> addDialog.dispose());

        addDialog.setLocationRelativeTo(bookList);
        addDialog.setVisible(true);
    }

    /**
     * Показывает диалоговое окно для управления пользователями.
     */
    private void showUserManagementDialog() {
        JDialog userDialog = new JDialog(bookList, "Управление пользователями", true);
        userDialog.setSize(500, 400);
        userDialog.setLayout(new BorderLayout());

        // Создание таблицы для пользователей
        String[] columns = {"Имя пользователя", "Пароль"};
        DefaultTableModel userModel = new DefaultTableModel(columns, 0);
        JTable userTable = new JTable(userModel);
        JScrollPane scrollPane = new JScrollPane(userTable);

        // Загрузка пользователей из файла
        File userFile = new File("./src/csv/user.csv");
        try (BufferedReader reader = new BufferedReader(new FileReader(userFile))) {
            String line;
            while ((line = reader.readLine()) != null) {
                String[] parts = line.split(",");
                if (parts.length == 2) {
                    userModel.addRow(parts);
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }

        // Панель для добавления нового пользователя
        JPanel addUserPanel = new JPanel(new GridLayout(3, 2));
        JTextField nameField = new JTextField();
        JPasswordField passwordField = new JPasswordField();
        JButton addUserButton = new JButton("Добавить пользователя");

        addUserPanel.add(new JLabel("Имя пользователя:"));
        addUserPanel.add(nameField);
        addUserPanel.add(new JLabel("Пароль:"));
        addUserPanel.add(passwordField);
        addUserPanel.add(new JLabel());
        addUserPanel.add(addUserButton);

        // Кнопка для удаления пользователя
        JButton deleteUserButton = new JButton("Удалить выбранного пользователя");

        // Панель кнопок
        JPanel buttonPanel = new JPanel();
        buttonPanel.add(deleteUserButton);

        // Добавление компонентов в диалог
        userDialog.add(scrollPane, BorderLayout.CENTER);
        userDialog.add(addUserPanel, BorderLayout.NORTH);
        userDialog.add(buttonPanel, BorderLayout.SOUTH);

        // Обработчик для добавления нового пользователя
        addUserButton.addActionListener(e -> {
            String name = nameField.getText().trim();
            String password = new String(passwordField.getPassword()).trim();

            if (name.isEmpty() || password.isEmpty()) {
                JOptionPane.showMessageDialog(userDialog, "Поля не должны быть пустыми.", "Ошибка", JOptionPane.ERROR_MESSAGE);
                return;
            }

            if (!name.matches("[a-zA-Zа-яА-Я]+")) {
                JOptionPane.showMessageDialog(userDialog, "Имя должно содержать только буквы.", "Ошибка", JOptionPane.ERROR_MESSAGE);
                return;
            }

            userModel.addRow(new String[]{name, password});
            nameField.setText("");
            passwordField.setText("");
        });

        // Обработчик для удаления выбранного пользователя
        deleteUserButton.addActionListener(e -> {
            int selectedRow = userTable.getSelectedRow();
            if (selectedRow != -1) {
                userModel.removeRow(selectedRow);
            } else {
                JOptionPane.showMessageDialog(userDialog, "Выберите пользователя для удаления.", "Ошибка", JOptionPane.ERROR_MESSAGE);
            }
        });

        // Добавляем обработчик закрытия окна для сохранения данных
        userDialog.addWindowListener(new WindowAdapter() {
            @Override
            public void windowClosing(WindowEvent e) {
                saveUpdatedUsers(userModel);
            }
        });

        userDialog.setLocationRelativeTo(bookList);
        userDialog.setVisible(true);
    }

    /**
     * Сохраняет обновлённые данные пользователей в файл.
     * @param userModel Модель данных таблицы пользователей.
     */
    private void saveUpdatedUsers(DefaultTableModel userModel) {
        File userFile = new File("./src/csv/user.csv");
        try (BufferedWriter writer = new BufferedWriter(new FileWriter(userFile))) {
            for (int i = 0; i < userModel.getRowCount(); i++) {
                String name = (String) userModel.getValueAt(i, 0);
                String password = (String) userModel.getValueAt(i, 1);
                writer.write(name + "," + password);
                writer.newLine();
            }
//            JOptionPane.showMessageDialog(bookList, "Пользователь удалён успешно.", "Информация", JOptionPane.INFORMATION_MESSAGE);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    /**
     * Проверяет аутентификацию пользователя перед запуском приложения.
     * @return {@code true}, если аутентификация прошла успешно, иначе {@code false}.
     */
    private boolean checkUserAuthentication() {
        File userFile = new File("./src/csv/user.csv");

        if (!userFile.exists()) {
            return showRegistrationDialog(true); // Обязательная регистрация
        }

        JDialog loginDialog = new JDialog(bookList, "Вход", true);
        loginDialog.setSize(300, 150);
        loginDialog.setLayout(new BorderLayout());

        JPanel inputPanel = new JPanel(new GridLayout(2, 2));
        JLabel nameLabel = new JLabel("Имя:");
        JTextField nameField = new JTextField();
        JLabel passwordLabel = new JLabel("Пароль:");
        JPasswordField passwordField = new JPasswordField();

        inputPanel.add(nameLabel);
        inputPanel.add(nameField);
        inputPanel.add(passwordLabel);
        inputPanel.add(passwordField);

        JPanel buttonPanel = new JPanel();
        JButton loginButton = new JButton("Войти");
        JButton cancelButton = new JButton("Отмена");
        buttonPanel.add(loginButton);
        buttonPanel.add(cancelButton);

        loginDialog.add(inputPanel, BorderLayout.CENTER);
        loginDialog.add(buttonPanel, BorderLayout.SOUTH);

        final boolean[] authenticated = {false};

        loginButton.addActionListener(e -> {
            String name = nameField.getText().trim();
            String password = new String(passwordField.getPassword()).trim();

            try (BufferedReader reader = new BufferedReader(new FileReader(userFile))) {
                String line;
                while ((line = reader.readLine()) != null) {
                    String[] parts = line.split(",");
                    if (parts.length == 2 && parts[0].equals(name) && parts[1].equals(password)) {
                        authenticated[0] = true;
                        break;
                    }
                }
            } catch (IOException ex) {
                ex.printStackTrace();
            }

            if (authenticated[0]) {
                loginDialog.dispose();
            } else {
                JOptionPane.showMessageDialog(loginDialog, "Неправильное имя или пароль", "Ошибка", JOptionPane.ERROR_MESSAGE);
            }
        });

        cancelButton.addActionListener(e -> System.exit(0));

        loginDialog.setLocationRelativeTo(null);
        loginDialog.setIconImage(new ImageIcon(getClass().getResource("/img/icon.png")).getImage());
        loginDialog.setVisible(true);

        return authenticated[0];
    }

    /**
     * Извлекает данные из модели таблицы.
     * @param model Модель таблицы.
     * @return Двумерный массив данных таблицы.
     */
    private String[][] getTableData(DefaultTableModel model) {
        int rowCount = model.getRowCount();
        int columnCount = model.getColumnCount();

        String[][] tableData = new String[rowCount][columnCount];

        for (int i = 0; i < rowCount; i++) {
            for (int j = 0; j < columnCount; j++) {
                tableData[i][j] = (String) model.getValueAt(i, j);
            }
        }

        return tableData;
    }

    /**
     * Включает сортировку строк в таблицах.
     */
    private void enableSorting() {
        tableTeams.setAutoCreateRowSorter(true);
        tableDrivers.setAutoCreateRowSorter(true);
        tableTracks.setAutoCreateRowSorter(true);
        tableCompetitions.setAutoCreateRowSorter(true);
    }

    /**
     * Сбрасывает результаты поиска и восстанавливает исходные данные.
     */
    private void resetSearch() {
        // Сбрасываем поле поиска
        searchField.setText("Поиск...");

        DefaultTableModel model = getSelectedModel();
        JTable table = getSelectedTable();

        if (model == null || table == null) return;

        // Возвращаем весь набор данных (убираем фильтр)
        if (table.getRowSorter() != null) {
            ((TableRowSorter<?>) table.getRowSorter()).setRowFilter(null);
        }
        table.setRowSorter(null);
    }

    /**
     * Выполняет поиск по ключевому слову в текущей таблице.
     */
    private void performSearch() {
        String keyword = searchField.getText().trim().toLowerCase();
        DefaultTableModel model = getSelectedModel();
        JTable table = getSelectedTable();

        if (model == null || table == null) return;

        TableRowSorter<DefaultTableModel> sorter = new TableRowSorter<>(model);
        if (!keyword.isEmpty() && !keyword.equals("поиск...")) {
            sorter.setRowFilter(RowFilter.regexFilter("(?i)" + keyword));
        } else {
            sorter.setRowFilter(null);
        }
        table.setRowSorter(sorter);
    }

    /**
     * Показывает диалоговое окно для регистрации пользователя.
     * @param isMandatory Флаг обязательной регистрации.
     * @return {@code true}, если регистрация выполнена успешно.
     */
    private boolean showRegistrationDialog(boolean isMandatory) {
        final boolean[] isRegistered = {false};

        // Создание диалогового окна
        JDialog registerDialog = new JDialog(bookList, "Регистрация пользователя", true);
        registerDialog.setSize(300, 200);
        registerDialog.setLayout(new BorderLayout());

        // Создание панели для ввода данных
        JPanel inputPanel = new JPanel(new GridLayout(4, 2));
        JLabel nameLabel = new JLabel("Имя:");
        JTextField nameField = new JTextField();
        JLabel passwordLabel = new JLabel("Пароль:");
        JPasswordField passwordField = new JPasswordField();

        inputPanel.add(nameLabel);
        inputPanel.add(nameField);
        inputPanel.add(passwordLabel);
        inputPanel.add(passwordField);

        // Создание панели с кнопками
        JPanel buttonPanel = new JPanel();
        JButton registerButton = new JButton("Регистрация");
        JButton cancelButton = new JButton("Отмена");

        buttonPanel.add(registerButton);
        buttonPanel.add(cancelButton);

        registerDialog.add(inputPanel, BorderLayout.CENTER);
        registerDialog.add(buttonPanel, BorderLayout.SOUTH);

        // Обработчик кнопки "Регистрация"
        registerButton.addActionListener(e -> {
            String name = nameField.getText();
            String password = new String(passwordField.getPassword());

            if (name.isEmpty() || password.isEmpty()) {
                JOptionPane.showMessageDialog(registerDialog, "Поля не должны быть пустыми.", "Ошибка", JOptionPane.ERROR_MESSAGE);
                return;
            }

            if (!name.matches("[a-zA-Zа-яА-Я]+")) {
                JOptionPane.showMessageDialog(registerDialog, "Имя должно содержать только буквы.", "Ошибка", JOptionPane.ERROR_MESSAGE);
                return;
            }

            try {
                File directory = new File("./src/csv");
                if (!directory.exists()) {
                    directory.mkdirs();
                }

                try (BufferedWriter writer = new BufferedWriter(new FileWriter("./src/csv/user.csv", true))) {
                    writer.write(name + "," + password);
                    writer.newLine();
                }

                JOptionPane.showMessageDialog(registerDialog, "Регистрация успешна!", "Информация", JOptionPane.INFORMATION_MESSAGE);
                isRegistered[0] = true;
                registerDialog.dispose();
            } catch (IOException ex) {
                ex.printStackTrace();
            }
        });

        // Обработчик кнопки "Отмена"
        cancelButton.addActionListener(e -> {
            registerDialog.dispose();
            if (isMandatory) {
                System.exit(0); // Закрыть приложение, если регистрация обязательна
            }
        });

        registerDialog.setLocationRelativeTo(null);
        registerDialog.setVisible(true);

        return isRegistered[0];
    }

    /**
     * Точка входа в приложение.
     * @param args Аргументы командной строки (не используются).
     */
    public static void main(String[] args) {
        // Запуск основного окна приложения
        new BookList().show();
    }
}
