
import sys
from PyQt6.QtWidgets import QApplication, QMainWindow, QPushButton
#importo la classe generata da compilatore ui py
#dal file demo1.py importo la classe ui_mainwindow
#ora posso usare ui_mainwindow

from ui_login import Ui_MainWindow
#creo la classe mainwindow che eredita le caratteristiche della classe  mainwindow
#scrivo la classe 
class MainWindow(QMainWindow):
      #nel costruttore inserisco  tutto il codice  per gestire la finestra e gli oggetti di caricare dentro
    def __init__(self):
        super(MainWindow, self).__init__()
        self.ui=Ui_MainWindow()
        self.ui.setupUi(self)
        
        self.setWindowTitle("My Window")


app = QApplication(sys.argv)

window = MainWindow()
window.show()

app.exec()       
#sempre uguale
