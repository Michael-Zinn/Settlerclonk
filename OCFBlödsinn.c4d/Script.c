/*
Dieses idiotische Hilfsobjekt gibt es nur, weil man bei GetOCF() keine ID angeben kann. Dieses Objekt erstellt jeden Objekttyp, �berpr�ft die OCFs und l�scht den Objekttyp wieder. W�rde der Siedlerclonk diese Objekte erstellen, so w�rde man wahnsinnig viele Siedlungspunkte kriegen. Die Punkte w�rden zwar nach ein paar Sekunden wieder verschwinden, jedoch k�nnte in dieser Zeit das Spielziel "Siedlungspunkte" diese Punkte entdecken und so die Runde beenden.

Falls in GWE3 der Befehl so aussieht:
GetOCF(pOBJ,C4ID);
Wird es dieses Objekt dann nicht mehr geben
*/

Initialize:
  SetOwner(-1);
  return(1);

public StimmtderOCF: // C4ID

  // Der Meteor und der Lavabrocken erzeugen Rauch, wenn er erstellt werden
  if(Or(
    Equal(Par(),METO),
    Equal(Par(),LAVA)
  ))
    return(1);

  if(BitAnd(
 // 2.OCF ermitteln 1.Objekt erstellen
    GetOCF(SetVar(2,CreateObject(Par()))),
    OCF_Collectible()
  ))
    // Ergebnis (BOOL) speichern
    SetVar(9,1);

  // Objekt wieder l�schen
  RemoveObject(Var(2));

  // BOOL zur�ckgeben
  return(Var(9));