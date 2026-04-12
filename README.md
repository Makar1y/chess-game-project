- [] ėjimo patikrinimas
- [] specialių atvejų patikrinimas (en passant, check(mate), castle)
- [] resign, draw, newGame funkcijos
- [] main game loop'o flow; player -> stockfish -> player -> ....
- [] stockFish su skirtingais elo
- [] išsaugoti ėjimus į vektorių
- [] stockFish gauna istoriją per stdin, sukuria ėjimą per stdout
- [] resign, draw turi būti užpildytas jau egzistuojantis overlay
- [] showResults() rodyti ėjimų istoriją, ėjimų skaičių, nugalėtoją, pan.
- [] sutvarkyti dizainą
- [] resign, draw, newGame mygtukai turi kviesti klases iš Game

![UML](UMLs/CLASSES_1.png)
---

kai reikės pradėti naują partiją vertėtų turėti smart pointerius, kad kai sunaikinama lenta nebūtų memeory leak dėl
senų pointerių arba galima daryti atskirą metodą kuris perstato figūras į originales pozicijas.

Draw daro per daug, gal pridėti dar vaikinių klasėių?
kažkur padėti:
	isLeftMousePressed
	isResignClicked
	isOfferDrawClicked
	isShowResultsClicked
	isOverlayButtonClicked
	getClickedBoardCell
	
figūros kažkodėl turi texture bleeding, jų apačioje yra artefaktai.

daug hardcodintų formatavimo reikšmių Draw.cpp
