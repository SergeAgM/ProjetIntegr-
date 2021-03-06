# -*- coding: cp1252 -*-
import sys                      # pour sys.exit
import serial                   # package de gestion du port serie

import scipy
import matplotlib
import numpy as np
from matplotlib import pyplot as plt # Pour l'affichage.
from scipy.io.wavfile import write
from scipy.fft import rfft, rfftfreq
from scipy.fft import irfft


# Configuration du port serie
serialPort = serial.Serial()    # creation de l'objet associe au port serie
serialPort.baudrate = 57600     # definition du baudrate
serialPort.port = 'COM4'        # selection du port serie
serialPort.timeout = 1          # temps d'attente avant d'abandonner une lecture

# Connexion au dsPIC
try:
    serialPort.open()               # on tente d'etablir la connexion
except:
    print('Echec de la connexion')  # En cas de probleme,
    sys.exit()                      # on ferme l'application

# Parametres de l'acquisition
sampleNb = 1000                     # Nombre d'echantillons a lire
samplePer = 1/15000 #200E-6         # Periode d'echantillonnage.
# A modifier avec celle du filtre. !!!!adc

# Creation d'un vecteur pour stocker les donnees
data = []
time = []

# Recuperation des donnees
serialPort.write(b's')              # Envoi du caractere de synchro
for a in range(sampleNb):
    tmp1 = serialPort.read(1)       # on lit 1 octet sur le port serie
    tmp2 = ord(tmp1)                # on le transforme en entier
    data.append(3.3*tmp2/255)       # on calcule la tension correspondante, en V
    time.append(a*samplePer)

# Deconnexion du dsPIC
serialPort.close()

# Affichage des donnees (domaine temporel)
plt.plot(time, data)
plt.grid()
plt.xlabel('t (s)')
plt.ylabel('CH1 (V)')
plt.show()

# Affichage de la transformée de Fourier (domaine fréquentiel)

yf = rfft(data)
xf = rfftfreq(sampleNb, samplePer) 
# Faire attention à bien mettre la même fréquence d'échantillonage que celle du filtre.

plt.plot(xf, np.abs(yf))
plt.show() # Ne pas oublier qu'on teste avec un filtre à 1100Hz.