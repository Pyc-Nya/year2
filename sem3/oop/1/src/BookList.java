import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.FileDialog;
import java.awt.LayoutManager;
import java.awt.event.*;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;

import javax.swing.BoxLayout;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JFrame;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.JTextField;
import javax.swing.JToolBar;
import javax.swing.JTabbedPane;
import javax.swing.table.DefaultTableModel;

public class BookList {
    // Объявления графических компонентов
    private JFrame bookList;
    private DefaultTableModel modelTeams;
    private DefaultTableModel modelDrivers;
    private DefaultTableModel modelTracks;
    private DefaultTableModel modelSorev;
    private JButton save;
    private JButton addFile;
    private JButton delFile;
    private JButton Trash;
    private JScrollPane scrollTeams;
    private JScrollPane scrollDrivers;
    private JScrollPane scrollTracks;
    private JScrollPane scrollSorev;
    private JTable tableTeams;
    private JTable tableDrivers;
    private JTable tableTracks;
    private JTable tableSorev;
    private JComboBox<String> searchCriteria;
    private JTextField searchField;
    private JButton filter;
	protected String fileName;
    

    public void show() {
        // Создание окна
        bookList = new JFrame("DATA BASE");
        bookList.setSize(600, 400);
        bookList.setLocation(100, 100);
        bookList.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

        // Создание кнопок и прикрепление иконок
        save = new JButton(new ImageIcon(getClass().getResource("/img/save.png")));
        save.setPreferredSize(new Dimension(40,30));
        save.setToolTipText("Сохранить изменения из вкладки");

        addFile = new JButton(new ImageIcon(getClass().getResource("/img/plus.png")));
        addFile.setPreferredSize(new Dimension(40,30));
        addFile.setToolTipText("Добавить файл во вкладку");

        delFile = new JButton(new ImageIcon(getClass().getResource("/img/minus.png")));
        delFile.setPreferredSize(new Dimension(40,30));
        delFile.setToolTipText("Удалить строку из вкладки");

        Trash = new JButton(new ImageIcon(getClass().getResource("/img/Trash.png")));
        Trash.setPreferredSize(new Dimension(40,30));
        Trash.setToolTipText("Удаление всей кладки");
        
        // Добавление кнопок на панель инструментов
        JToolBar toolBar = new JToolBar(JToolBar.VERTICAL); // Вертикальная ориентация
        toolBar.setLayout((LayoutManager) new BoxLayout(toolBar, BoxLayout.Y_AXIS)); // Вертикальное расположение элементов

        new JButton("addFile");
        new JButton("Save");
        new JButton("Delete");
        new JButton("Trash");
        toolBar.add(save);
        toolBar.add(addFile);
        toolBar.add(delFile);
        toolBar.add(Trash);

        // Размещение панели инструментов
        bookList.setLayout(new BorderLayout());
        bookList.add(toolBar, BorderLayout.WEST);

        // Создание таблицы для команд
        String[] columnsTeams = {"Команда", "Страна"};
        String[][] dataTeams = {};
        modelTeams = new DefaultTableModel(dataTeams, columnsTeams);
        tableTeams = new JTable(modelTeams);
        scrollTeams = new JScrollPane(tableTeams);

        // Создание таблицы для гонщиков
        String[] columnsDrivers = {"Гонщик", "Команда", "Очки"};
        String[][] dataDrivers = {};
        modelDrivers = new DefaultTableModel(dataDrivers, columnsDrivers);
        tableDrivers = new JTable(modelDrivers);
        scrollDrivers = new JScrollPane(tableDrivers);

        // Создание таблицы для трасс
        String[] columnsTracks = {"Трасса", "Местоположение"};
        String[][] dataTracks = {};
        modelTracks = new DefaultTableModel(dataTracks, columnsTracks);
        tableTracks = new JTable(modelTracks);
        scrollTracks = new JScrollPane(tableTracks);
        
        //создание таблицы для соревнований
        String[] columnsSorev= {"Трасса","Дата проведения","Начало заезда"};
        String[][] dataSorev= {};
        modelSorev= new DefaultTableModel(dataSorev,columnsSorev);
        tableSorev= new JTable(modelSorev);
        scrollSorev=new JScrollPane(tableSorev);

        // Создание вкладок с таблицами
        JTabbedPane tabbedPane = new JTabbedPane();
        tabbedPane.addTab("Команды", scrollTeams);
        tabbedPane.addTab("Гонщики", scrollDrivers);
        tabbedPane.addTab("Трассы", scrollTracks);
        tabbedPane.addTab("Соревнования", scrollSorev);

        // Добавление вкладок в окно
        bookList.add(tabbedPane, BorderLayout.CENTER);

        // Подготовка компонентов поиска
        searchCriteria = new JComboBox<>(new String[]{"Команды", "Гонщики", "Трассы","Соревнования"});
        searchField = new JTextField("Ключевое слово", 30);
        filter = new JButton("Поиск");
        
        // Панель поиска
        JPanel searchPanel = new JPanel();
        searchPanel.add(searchCriteria);
        searchPanel.add(searchField);
        searchPanel.add(filter);

        // Добавление панели поиска внизу окна
        bookList.add(searchPanel, BorderLayout.SOUTH);
        
        //Сохранение файла
        save.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent arg0) {
                // Окно выбора вкладки для сохранения данных
                String[] options = {"Команды", "Гонщики", "Трассы", "Соревнования"};
                int tabChoice = JOptionPane.showOptionDialog(
                    bookList, 
                    "Выберите вкладку, данные которой нужно сохранить:",
                    "Выбор вкладки для сохранения",
                    JOptionPane.DEFAULT_OPTION, 
                    JOptionPane.QUESTION_MESSAGE, 
                    null, 
                    options, 
                    options[0]
                );

                if (tabChoice == -1) return; // Если пользователь закрыл окно выбора

                // Диалоговое окно для сохранения файла
                FileDialog saveDialog = new FileDialog(bookList, "Сохранение данных", FileDialog.SAVE);
                saveDialog.setFile("data.csv"); // Укажите исходное имя файла
                saveDialog.setVisible(true);
                
                String directory = saveDialog.getDirectory();
                String selectedFile = saveDialog.getFile();
                
                if (directory == null || selectedFile == null) {
                    return; // Если пользователь нажал «отмена»
                }

                String fileName = directory + selectedFile;

                // Определяем выбранную таблицу
                DefaultTableModel selectedModel;
                switch (tabChoice) {
                    case 0: selectedModel = modelTeams; break;
                    case 1: selectedModel = modelDrivers; break;
                    case 2: selectedModel = modelTracks; break;
                    case 3: selectedModel = modelSorev; break;
                    default: return;
                }

                // Сохранение данных в выбранный файл
                try (BufferedWriter writer = new BufferedWriter(new FileWriter(fileName))) {
                    for (int i = 0; i < selectedModel.getRowCount(); i++) {
                        for (int j = 0; j < selectedModel.getColumnCount(); j++) {
                            writer.write((String) selectedModel.getValueAt(i, j)); // Запись значения ячейки
                            if (j < selectedModel.getColumnCount() - 1) {
                                writer.write(","); // Разделитель
                            }
                        }
                        writer.write("\n"); // Переход на новую строку
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        });

        //удаление строки во вкладке
        delFile.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent event) {
                // Получение активной вкладки
                int selectedTab = tabbedPane.getSelectedIndex();
                
                JTable selectedTable;
                DefaultTableModel selectedModel;

                // Определяем, какая таблица активна
                switch (selectedTab) {
                    case 0:
                        selectedTable = tableTeams;
                        selectedModel = modelTeams;
                        break;
                    case 1:
                        selectedTable = tableDrivers;
                        selectedModel = modelDrivers;
                        break;
                    case 2:
                        selectedTable = tableTracks;
                        selectedModel = modelTracks;
                        break;
                    case 3:
                        selectedTable = tableSorev;
                        selectedModel = modelSorev;
                        break;
                    default:
                        return;
                }

                // Получаем индекс выбранной строки в активной таблице
                int selectedRow = selectedTable.getSelectedRow();

                // Проверяем, выбрана ли строка
                if (selectedRow != -1) {
                    selectedModel.removeRow(selectedRow); // Удаляем строку
                } else {
                    // Если строка не выбрана, выводим предупреждение
                    JOptionPane.showMessageDialog(bookList, "Пожалуйста, выберите строку для удаления.", "Ошибка", JOptionPane.ERROR_MESSAGE);
                }
            }
        });
        //удаление данных вкладки
        Trash.addActionListener (new ActionListener()
        {
        public void actionPerformed (ActionEvent event) 
        {
        	// Окно выбора вкладки для сохранения данных
            String[] options = {"Команды", "Гонщики", "Трассы", "Соревнования"};
            int tabChoice = JOptionPane.showOptionDialog(
                bookList, 
                "Выберите вкладку, данные которой нужно удалить:",
                "Выбор вкладки для удаления",
                JOptionPane.DEFAULT_OPTION, 
                JOptionPane.QUESTION_MESSAGE, 
                null, 
                options, 
                options[0]
            );
         // Определяем выбранную таблицу
            DefaultTableModel selectedModel;
            switch (tabChoice) {
                case 0: selectedModel = modelTeams; break;
                case 1: selectedModel = modelDrivers; break;
                case 2: selectedModel = modelTracks; break;
                case 3: selectedModel = modelSorev; break;
                default: return;
            }
            int rows = selectedModel.getRowCount();
            for (int i = 0; i < rows; i++) selectedModel.removeRow(0); // Очистка таблицы
        }});
        //добавление данных в библиотеку
        addFile.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent arg0) {
                FileDialog addFile = new FileDialog(bookList, "Добавление данных", FileDialog.LOAD);
                addFile.setVisible(true); // Отобразить запрос пользователю
                
                String directory = addFile.getDirectory();
                String selectedFile = addFile.getFile();
                
                if (directory == null || selectedFile == null) {
                    return; // Если пользователь нажал «отмена», прекратить выполнение
                }
                
                String fileName = directory + selectedFile;
                
                // Окно выбора вкладки для добавления данных
                String[] options = {"Команды", "Гонщики", "Трассы", "Соревнования"};
                int tabChoice = JOptionPane.showOptionDialog(
                    bookList, 
                    "Выберите вкладку, в которую нужно добавить данные:",
                    "Выбор вкладки",
                    JOptionPane.DEFAULT_OPTION, 
                    JOptionPane.QUESTION_MESSAGE, 
                    null, 
                    options, 
                    options[0]
                );
                
                if (tabChoice == -1) return; // Если пользователь закрыл окно выбора
                
                try (BufferedReader reader = new BufferedReader(new FileReader(fileName))) {
                    String line;
                    
                    DefaultTableModel selectedModel;
                    switch (tabChoice) {
                        case 0: selectedModel = modelTeams; break;
                        case 1: selectedModel = modelDrivers; break;
                        case 2: selectedModel = modelTracks; break;
                        case 3: selectedModel = modelSorev; break;
                        default: return;
                    }

                    // Чтение и добавление данных в выбранную таблицу
                    while ((line = reader.readLine()) != null) {
                        String[] fields = line.split(","); // Укажите разделитель для CSV
                        selectedModel.addRow(fields); // Добавление строки в таблицу
                    }
                    
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        });

        // Визуализация экранной формы
        bookList.setVisible(true);
    }
    
    private class MyException extends Exception {
    	public MyException() {
    	super ("Вы не ввели название книги для поиска");
    	}}
    private void checkName (JTextField bName) throws MyException,NullPointerException
    	{
    	String sName = bName.getText();
    	if (sName.contains("Название книги")) throw new MyException();
    	if (sName.length() == 0) throw new MyException();
    	} 

    public static void main(String[] args) {
        // Создание и отображение экранной формы
    	 
        new BookList().show();
    }
}




