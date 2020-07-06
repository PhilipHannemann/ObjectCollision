---- Anwendung "Quantum" ----
----    Demo Projekt     ----

Die Anwendung kann mit Hilfe der "START.bat" oder direkt �ber "\binaries\x64\Quantum.exe" gestartet werden.

Zur Ausf�hrung wird ein x64 System ben�tigt.

Das Archiv muss vor dem Ausf�hren entpackt werden!

-- Eingabeoptionen --

F1...........................GUI ein/ausblenden
Rechte Maustaste (halten)....Kamera rotieren
Mausrad......................Zoom
W,A,S,D......................Kamera bewegen
Q,E..........................Kamerah�he �ndern
Linke Maustaste..............Objekt ausw�hlen
Leertaste....................Objekt ansto�en 

- Objekte ansto�en -
Durch Halten der Leertaste kann die St�rke des Sto�es gew�hlt werden. Je l�nger die Taste gehalten wird, desto st�rker ist der Sto�.
Beim Loslassen der Leertaste wird die Kraft auf den K�rper angewendet, �ber dem sich aktuell der Mauscursor befindet.
Die Kraft wird gleichm��ig auf alle Partikel verteilt und in Richtung der aktuellen Blickrichtung angewendet.


-- GUI --

- Fenster : "Einstellungen" -

Hier kann die Aufl�sung des Fensters eingestellt, sowie der "Vollbild" Modus gew�hlt werden.
Ebenso ist es m�glich den Monitor zu bestimmen auf dem die Anwendung dargestellt wird.
Durch Klick auf "Speichern" werden die Einstellungen angewendet und dauerhaft gespeichert.
Durch Klick auf "Beenden" wird die Anwendung geschlossen.

- Fenster : "Debug" -

Im Abschnitt "Debug Draw" kann das Anzeigen von Zusatzinformationen aktiviert/deaktiviert werden:
	Wireframe: 		   Darstellung der gesamten Szene als Drahtgittermodell
	Goalpos:   		   Visualisierung der Zielpositionen und Massezentren der Soft Bodies
	Kollisionsh�llen:  Visualisierung der untersten Ebene des Sphere Trees, die f�r die aktuelle Cursorposition getestet wurden.

Im Abschnitt "World" kann mit dem ersten Button die Simulation pausiert/fortgesetzt werden. 
Mit dem Zweiten Button ist die schrittweise Ausf�hrung m�glich.
Wichtig: die Szene ist nach dem Start immer pausiert!

Mit den 3 Buttons im unteren Teil k�nnen verschiedene Testszenen geladen und zur�ckgesetzt werden.

- Fenster : "Soft Body Config" -

Dieses Fenster wird nur angezeigt, wenn ein Soft Body ausgew�hlt wurde. (Auswahl mit Linker Maustaste, erkennbar an blauem Wireframe Overlay)
Es k�nnen die Physikparameter des gew�hlten Soft Bodys ge�ndert werden.



-- Testszenen --

Alle Soft Bodies verwenden den Deformationsmodus "Linear".
Die voreingestellten Alpha- und Beta-Werte werden f�r alle Objekte durch deren Farbe visualisiert und sind auch im "Soft Body Config"-Fenster einsehbar/�nderbar.

- W�rfel -
Hier soll der Aufprall von 5 Meshes mit verschiedenen Alpha- und Beta-Werten verglichen werden. 
Bei den Meshes handelt es sich um W�rfel mit abgerundeten Kanten. (#Vertices: 98, #Faces: 192)

- Affe -
Hier soll der Aufprall eines komplexen, konkaven Meshes getestet werden.
Beim Mesh handelt es sich um einen Affenkopf (Vorgabemodell der Software: "Blender") mit hohem Detailgrad. (#Vertices: 7958, #Faces: 15744)
Standardm��ig ist Alpha = 0.7 und Beta = 0.6 gesetzt.
Durch Verwendung des "Soft Body Config"-Fensters k�nnen vor dem Aufprall auch andere Werte gew�hlt werden.

- Billard -
Hier sollen Kollisionen zwischen Soft Bodies getestet werden. Auf demr "Billardtisch" befinden sich 9 Objekte mit jeweils verschiedenen Physikeigenschaften.
Bei 5 Meshes handelt es sich um W�rfel mit abgerundeten Kanten. (#Vertices: 98, #Faces: 192)
Bei 4 Meshes handelt es sich um Kugeln. (#Vertices: 162, #Faces: 320)
Mit Hilfe der Leertaste k�nnen die Objekte angesto�en werden.

