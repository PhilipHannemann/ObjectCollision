---- Anwendung "Quantum" ----
----    Demo Projekt     ----

Die Anwendung kann mit Hilfe der "START.bat" oder direkt über "\binaries\x64\Quantum.exe" gestartet werden.

Zur Ausführung wird ein x64 System benötigt.

Das Archiv muss vor dem Ausführen entpackt werden!

-- Eingabeoptionen --

F1...........................GUI ein/ausblenden
Rechte Maustaste (halten)....Kamera rotieren
Mausrad......................Zoom
W,A,S,D......................Kamera bewegen
Q,E..........................Kamerahöhe ändern
Linke Maustaste..............Objekt auswählen
Leertaste....................Objekt anstoßen 

- Objekte anstoßen -
Durch Halten der Leertaste kann die Stärke des Stoßes gewählt werden. Je länger die Taste gehalten wird, desto stärker ist der Stoß.
Beim Loslassen der Leertaste wird die Kraft auf den Körper angewendet, über dem sich aktuell der Mauscursor befindet.
Die Kraft wird gleichmäßig auf alle Partikel verteilt und in Richtung der aktuellen Blickrichtung angewendet.


-- GUI --

- Fenster : "Einstellungen" -

Hier kann die Auflösung des Fensters eingestellt, sowie der "Vollbild" Modus gewählt werden.
Ebenso ist es möglich den Monitor zu bestimmen auf dem die Anwendung dargestellt wird.
Durch Klick auf "Speichern" werden die Einstellungen angewendet und dauerhaft gespeichert.
Durch Klick auf "Beenden" wird die Anwendung geschlossen.

- Fenster : "Debug" -

Im Abschnitt "Debug Draw" kann das Anzeigen von Zusatzinformationen aktiviert/deaktiviert werden:
	Wireframe: 		   Darstellung der gesamten Szene als Drahtgittermodell
	Goalpos:   		   Visualisierung der Zielpositionen und Massezentren der Soft Bodies
	Kollisionshüllen:  Visualisierung der untersten Ebene des Sphere Trees, die für die aktuelle Cursorposition getestet wurden.

Im Abschnitt "World" kann mit dem ersten Button die Simulation pausiert/fortgesetzt werden. 
Mit dem Zweiten Button ist die schrittweise Ausführung möglich.
Wichtig: die Szene ist nach dem Start immer pausiert!

Mit den 3 Buttons im unteren Teil können verschiedene Testszenen geladen und zurückgesetzt werden.

- Fenster : "Soft Body Config" -

Dieses Fenster wird nur angezeigt, wenn ein Soft Body ausgewählt wurde. (Auswahl mit Linker Maustaste, erkennbar an blauem Wireframe Overlay)
Es können die Physikparameter des gewählten Soft Bodys geändert werden.



-- Testszenen --

Alle Soft Bodies verwenden den Deformationsmodus "Linear".
Die voreingestellten Alpha- und Beta-Werte werden für alle Objekte durch deren Farbe visualisiert und sind auch im "Soft Body Config"-Fenster einsehbar/änderbar.

- Würfel -
Hier soll der Aufprall von 5 Meshes mit verschiedenen Alpha- und Beta-Werten verglichen werden. 
Bei den Meshes handelt es sich um Würfel mit abgerundeten Kanten. (#Vertices: 98, #Faces: 192)

- Affe -
Hier soll der Aufprall eines komplexen, konkaven Meshes getestet werden.
Beim Mesh handelt es sich um einen Affenkopf (Vorgabemodell der Software: "Blender") mit hohem Detailgrad. (#Vertices: 7958, #Faces: 15744)
Standardmäßig ist Alpha = 0.7 und Beta = 0.6 gesetzt.
Durch Verwendung des "Soft Body Config"-Fensters können vor dem Aufprall auch andere Werte gewählt werden.

- Billard -
Hier sollen Kollisionen zwischen Soft Bodies getestet werden. Auf demr "Billardtisch" befinden sich 9 Objekte mit jeweils verschiedenen Physikeigenschaften.
Bei 5 Meshes handelt es sich um Würfel mit abgerundeten Kanten. (#Vertices: 98, #Faces: 192)
Bei 4 Meshes handelt es sich um Kugeln. (#Vertices: 162, #Faces: 320)
Mit Hilfe der Leertaste können die Objekte angestoßen werden.

