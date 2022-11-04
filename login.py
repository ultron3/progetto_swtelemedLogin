
import sys
from PyQt6.QtWidgets import QApplication, QMainWindow, QPushButton
#importo la classe generata da compilatore ui py
#dal file demo1.py importo la classe ui_mainwindow
#ora posso usare ui_mainwindow
import jwt
from ui_login import Ui_MainWindow
#creo la classe mainwindow che eredita le caratteristiche della classe  mainwindow
#scrivo la classe 
import mysql.connector

mydb = mysql.connector.connect(
  host="localhost",
  user="root",
  password="telemedicina2123"
  
)

print(mydb)


class MainWindow(QMainWindow):
      #nel costruttore inserisco  tutto il codice  per gestire la finestra e gli oggetti di caricare dentro
    def __init__(self):
        super(MainWindow, self).__init__()
        self.ui=Ui_MainWindow()
        self.ui.setupUi(self)
        
        
        self.setWindowTitle("WIOTELEMED")


app = QApplication(sys.argv)

window = MainWindow()
window.show()

app.exec()       
#sempre uguale

mycursor = mydb.cursor()

mycursor.execute("SELECT * FROM db_pharm.tboxlog")

myresult = mycursor.fetchall()

for a in myresult:
  print(a)


mycursor = mydb.cursor()

mycursor.execute("SELECT * FROM db_pharm.tbox")

myresult = mycursor.fetchall()

for b in myresult:
  print(b)

mycursor = mydb.cursor()

mycursor.execute("SELECT * FROM db_pharm.tpharm")

myresult = mycursor.fetchall()

for b in myresult:
  print(b)

mycursor = mydb.cursor()

mycursor.execute("SELECT * FROM db_pharm.tuser")

myresult = mycursor.fetchall()

for x in myresult:
  print(x)