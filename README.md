# 🎮 Push Or Rage - Console d'Arcade Embarquée

![C](https://img.shields.io/badge/C-00599C?style=for-the-badge&logo=c&logoColor=white)
![C#](https://img.shields.io/badge/C%23-239120?style=for-the-badge&logo=c-sharp&logoColor=white)
![WPF](https://img.shields.io/badge/WPF-512BD4?style=for-the-badge&logo=windows&logoColor=white)
![STM32](https://img.shields.io/badge/STM32-03234B?style=for-the-badge&logo=stmicroelectronics&logoColor=white)
![UART](https://img.shields.io/badge/Protocol-UART_Serial-orange?style=for-the-badge)
![Doxygen](https://img.shields.io/badge/Docs-Doxygen-blue?style=for-the-badge)

**Push Or Rage** est un projet de co-design matériel et logiciel (Systèmes Embarqués & Informatique). Il s'agit d'une console d'arcade interactive composée d'une carte microcontrôleur STM32 et d'une Interface Homme-Machine (IHM) développée en C# WPF sur PC.

## 🚀 Fonctionnalités du Projet

Le système propose deux modes de jeu classiques :
* ⚡ **Jeu de Réflexe :** Des LEDs s'allument aléatoirement. Le joueur doit appuyer sur le bouton correspondant le plus vite possible pour marquer des points avant la fin du temps imparti.
* 🧠 **Jeu de Mémoire (Simon) :** Le système affiche une séquence lumineuse que le joueur doit mémoriser et reproduire sans erreur. La difficulté augmente à chaque niveau.

**Fonctionnalités matérielles :**
* **PWM & ADC :** La luminosité des LEDs est réglable en temps réel via un potentiomètre physique (Conversion Analogique-Numérique et modulation de largeur d'impulsion).
* **Anti-rebond :** Traitement logiciel robuste des appuis sur les boutons d'arcade physiques.
* **Machine à états :** Architecture logicielle non bloquante en C embarqué (Full LL Drivers).

**Fonctionnalités logicielles (IHM PC) :**
* Architecture MVVM (Model-View-ViewModel) en C#.
* Configuration de la partie depuis l'ordinateur (Choix du mode, temps de jeu).
* Affichage en temps réel du score, du temps restant et de l'état des composants matériels.

---

## 🛠️ Matériel Requis

* **Microcontrôleur :** Carte Nucleo STM32L053
* **Actionneurs :** 4 LEDs (Bleu, Jaune, Vert, Rouge) connectées sur des broches compatibles PWM (Timers).
* **Capteurs :** 4 Boutons d'arcade (Pull-up) + 1 Potentiomètre linéaire.
* **Communication :** Câble USB (Liaison Série UART à 115200 bauds).

---

## 📡 Protocole de Communication (UART)

Le système repose sur une communication bidirectionnelle asynchrone par interruptions entre la STM32 et le PC.

**Du PC vers la STM32 (Commandes) :**
* `CMD:START:Reflexe:60\n` -> Lance une partie de Réflexe de 60 secondes.
* `CMD:START:Memoire:0\n` -> Lance une partie de Mémoire.

**De la STM32 vers le PC (Télémétrie) :**
* `SCR:15\n` -> Met à jour le score du joueur (15 points).
* `TIM:45\n` -> Met à jour le temps restant (45 secondes).
* `MSG:Preparez-vous !\n` -> Met à jour la console de l'interface graphique.
* `LED:BLEU:1\n` -> Indique que le bouton bleu est actuellement pressé.

---

## 💻 Installation et Lancement

### 1. Partie Embarquée (C / STM32CubeIDE)
1. Ouvrir le projet `Push_Or_Rage` dans STM32CubeIDE.
2. Compiler le projet (`Project > Build Project`).
3. Flasher le programme sur la carte Nucleo via le bouton *Run/Debug*.

### 2. Partie IHM (C# / Visual Studio)
1. Ouvrir la solution Visual Studio.
2. Compiler et lancer l'application WPF.
3. Dans l'interface, sélectionner le port série COM correspondant à la carte Nucleo et cliquer sur "Connecter".
4. Choisir le mode de jeu, la durée, et cliquer sur "Jouer" !

---

## 📚 Documentation

Le code source embarqué est entièrement documenté selon les standards **Doxygen**. 
Pour générer ou consulter la documentation HTML des fonctions, référez-vous au dossier `Core/Src/doc/html/index.html`.

---
*Projet réalisé dans le cadre du module de Systèmes Embarqués / TP Microcontrôleurs.*
