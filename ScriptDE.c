/*-- Siedlerclonk *--/ ;-P

  Version 1.9.3b
  von Michael Zinn (Horus)

  Lokale Variablen:
    Local [0] : Transportstart (X-Achse/Objekt)
    Local [1] : Transportstart (Y-Achse/-1(Objekt))
    Local [2] : Transportziel (X-Achse/Objekt)
    Local [3] : Transportziel (Y-Achse/-1)
    Local [4] : Gebäude, welches Energie benötig (ist eine Var)
    Local [5] : Der Clonk transportiert gerade
    Local [4] : ID grün
    Local [5] : Blau start x
    Local [6] : blau start y
    Local [7] : blau ziel x
    Local [8] : blau ziel y
    Local [9] : ID blau

    Ich mache doch nicht wegen einer Variable ein Hilfsobjekt!
      Physical "Float" (Bitmaske):
        0( 1)=check green
        1( 2)=check blue/green2
        2( 4)=check green3
        3( 8)=blue converted to green
        4(16)=Overwatch
FLOAT : Bitmaske:
        0(  1)=check green
        1(  2)=check blue/green2
        2(  4)=check green3
        3(  8)=blue converted to green
        4( 16)=Overwatch
        5( 32)=Sell green
        6( 64)=Sell blue
        7(128)=green id except
        8(256)=blue id except

Symbole:
Für den Siedlerclonk (SC7K)
S0 7K	| Resume Transport
S1 7K	| Transport Start
S2 7K	| Transport Start Set
S3 7K	| Transport Target
S4 7K	| Transport Target Set
S5 7K	| Advanced Options
S6 7K	| SELECTED BlueOptions
S7 7K	| Green ID
S8 7K	| Check 
S9 7K	|  Check green not
SA 7K	|   Check green
SB 7K	|  Check blue not
-- --	| -
SD 7K	|   Check blue
SE 7K	| Green Bad Resume
SF 7K	| Blue Bad Resume
SG 7K	| Blue options
SH 7K	|  Blue Start Not Set
SI 7K	|   Blue Start Set
SJ 7K	|  Blue Target Not Set
SK 7K	|   Blue Target Set
SL 7K	|  Blue Resume
SM 7K	|  Blue ID
SN 7K	| Conv. Not
SO 7K	|  Conv. Yes
SP 7K	| Klammer auf
SQ 7K	| Klammer zu
SR 7K	| SELECTED Blue ID
ST 7K	| ID All 
SU 7K	| Blue Options impossible
SV 7K	| Green Start2
SW 7K	| Green Start2 Set
SX 7K	| Green Start3
SY 7K	| Green Start3 Set
SZ 7K	| Overwatch (ist auch gleichzeitig das Symbol über den Siedlerclonks)
===================
X1 7K	| SELECTED Check
X2 7K	| SELECTED Green ID
X3 7K	| Communication
X4 7K	| SELECTED Communication
X5 7K	| Auto sell not
X6 7K	| Auto sell yes
X7 7K	| Transport except not
X8 7K	| Transport except yes


Hilfsobjekt:
S_ 7K	| Idiotisches Hilfsobjekt für OCFs


#################################################################
##            ###################################################
##   SCRIPT   ###################################################
##            ###################################################
#################################################################

-* Basiert auf dem Clonk und benötigt das Betterpack -----------*/

#include CLNK
#include BA7K

protected Initialize:
  SetAction("Walk");
  SetDir(Random(2));

  // Wenn ich schon Initialize überlade kann ich es auch verbessern
  SetPhase(Random(8));

  // Bitmaske zurücksetzen
  SetPhysical("Float");

  return(1);


/* Überladbare Bestätigungssounds (z.B.: für #include SC7K) ###*/

SoundConfirm:
  Debug("SoundConfirm");
  return(Sound("SettlerConfirm*"));
  return(1);

SoundCommandFailure:
  Debug("SoundCommandFailure");
  Sound("CommandFailure1");
  return(1);

SoundWorkDone:
  // Ich will hier kein * benutzen

  // von matthes
  if(Random(2))
    return(Sound("Fertig"));

  // von mir
  return(Sound("HorusFertig"));

SoundMenuSelect:
  return(Sound("ControlRate"));


/* Bitmaskenverwaltung ########################################*/

SetBit: // Bit, Wert (BOOL)
  // ist schon drin: nichts tun
  if(And(
    Par(1),
    BitAnd(GetPhysical("Float"),Pow(2,Par()))
  ))
    return(GetPhysical("Float"));

  // ist schon draußen: nichts tun
  if(And(
    Not(Par(1)),
    Not(BitAnd(GetPhysical("Float"),Pow(2,Par())))
  ))
    return(GetPhysical("Float"));

  // ist nicht drin und soll rein: rein tun
  if(And(
    Par(1),
    Not(BitAnd(GetPhysical("Float"),Pow(2,Par())))
  ))
    return(SetPhysical(
      "Float",
      Sum(
        GetPhysical("Float"),
        Pow(2,Par())
      )
    ));

  // ist drin und soll raus: raus tun
  if(And(
    Not(Par(1)),
    BitAnd(GetPhysical("Float"),Pow(2,Par()))
  ))
    return(SetPhysical(
      "Float",
      Sub(
        GetPhysical("Float"),
        Pow(2,Par())
      )
    ));

  // Fehler
  return();


GetBit: // Bit
  if(BitAnd(GetPhysical("Float"),Pow(2,Par())))
    return(1);
  return();


/* Lässt sich praktischer Weise ganz einfach für Testzwecke ändern */

StartAreaWidth:
  return(100);

StartAreaHeight:
  return(50);



/* Kontext ####################################################*/

/* Baumenü öffnen ---------------------------------------------*/

public ContextConstruction: // C4Object *pCaller
  [Bauauftrag|Image=CXCN|Condition=HasKnowledge]
  MenuConstruction();
  return(1);

public HasKnowledge:
  return(GetPlrKnowledge(GetOwner(),0,0,C4D_Structure()));


/* Bei Baubefehl bestätigen */
protected ControlCommand:
  // Overwatch deaktivieren
  DeactivateOverwatch();
  SetBit(9);
  // Bestätigen
  if(SEqual( Par(0),"Construct" )) SoundConfirm();
  // normal weitermachen
  return(0);


/* Baumenü, bei Auswahl bestätigen */
protected MenuConstruction:
  // Bauauftragmenü erzeugen und mit Bauplänen füllen
  CreateMenu(CXCN,this(),this(),1,"Keine Baupläne verfügbar.");
  SetVar(0,-1);
  while(SetVar(1,GetPlrKnowledge(
    GetOwner(),0,
    IncVar(0),
    C4D_Structure() 
  )))
    AddMenuItem("Bauauftrag: %s","SetOrderConstruction",Var(1));
  return(1);


/* Bauen, aber mit Bestätigung */
public SetOrderConstruction:
  DeactivateOverwatch();
  SetBit(9);
  SoundConfirm();
  // Befehl mit Gebäudetyp setzen
  AddCommand(this(),"Construct",0, 0,0, 0,0, Par(0) );
  return(1);


/* Zurück zur Basis (mit Sound) -------------------------------*/

public ContextHome: // C4Object *pCaller
  [Zurück zur Basis|Image=CXHM|Condition=HasBase]
  SoundConfirm();
  DeactivateOverwatch();
  SetBit(9);
  SetCommand(this(),"Home");
  return(1);


/* Gebäude mit Energie versorgen ------------------------------*/

public ContextSupplyWithEnergy:
  [Energieversorgung|Image=ENRG|Condition=FindEnergyNeeder]
  SoundConfirm();
  AddCommand(this(),"Energy",Local(4));  
  // Local(4) war nur function-übergreifend und wird gelöscht
  SetLocal(4);
  return(1);

FindEnergyNeeder: // Braucht Energy und die Regel ENRG is set
  if(NeedsPowerSupply(SetVar(0,
    FindObject(0,-1,-1,0,0,OCF_PowerConsumer()) 
  )))
    return(SetLocal( 4,Var(0) )); // Local(4) ist eine Var
  // Es gibt keinen Energieverbraucher
  return();


/* Abgebrochenen Transport wieder aufnehmen -------------------*/

public ContextResumeTransport:
  [Transport fortsetzen|Image=S07K|Condition=TransportIsPossible]
  SoundConfirm();
//  SetBit(4,1);
  TryTransport();
  return(1);
/*TransportIsPossible:
  if(And(
    Not(Local(x5)),
    TransportIsPossible()
  ))
    return(1);
  return(0);*/

public ContextBadResumeTransport:
  [Transport fortsetzen|Image=SE7K|Condition=BadTransportConditions]
  SoundCommandFailure();
//  SetBit(4,1);
  Message("Start und Ziel|sind identisch!",this());
  return(1);

BadTransportConditions:
  if(And(
    TransportStartIsSet(),
    And(
      TransportTargetIsSet(),
      StartEqualTarget()
    )
  ))
    return(1);
  return();

BadTransport2Conditions:
  if(And(
    TransportStart2IsSet(),
    And(
      TransportTarget2IsSet(),
      Start2EqualTarget2()
    )
  ))
    return(1);
  return();

/* Transportstart festlegen -----------------------------------*/

// Grau darstellen
public ContextTransportStartno:                                 
  [Transportstart|Image=S17K|Condition=TransportStartNotSet]
  // Start speichern
  return(SetTransportStart());

// Grün darstellen
public ContextTransportStartyes:
  [Transportstart|Image=S27K|Condition=TransportStartIsSet]
  return(SetTransportStart());

public TransportStartNotSet:
  if(Not( TransportStartIsSet() )) return(1);
  return(0);


// Grau darstellen
public ContextTransportStart2no:                                 
  [2.Transportstart|Image=SV7K|Condition=ConT2SNo]
  // Start speichern
  return(SetTransportStart2());

ConT2SNo:
  if(And(
    GetBit(3),
    Not( TransportStart2IsSet() )
  )) return(1);
  return();

// Grün darstellen
public ContextTransportStart2yes:
  [2.Transportstart|Image=SW7K|Condition=ConT2SYes]
  return(SetTransportStart2());

ConT2SYes:
  if(And(
    GetBit(3),
    TransportStart2IsSet()
  )) return(1);
  return();


// Grau darstellen
public ContextTransportStart3no:                                 
  [3.Transportstart|Image=SX7K|Condition=ConT3SNo]
  // Start speichern
  return(SetTransportTarget2());

ConT3SNo:
  if(And(
    GetBit(3),
    Not( TransportTarget2IsSet() )
  )) return(1);
  return();

// Grün darstellen
public ContextTransportStart3yes:
  [3.Transportstart|Image=SY7K|Condition=ConT3SYes]
  return(SetTransportTarget2());

ConT3SYes:
  if(And(
    GetBit(3),
    TransportTarget2IsSet()
  )) return(1);
  return();


/* Transportziel festlegen ------------------------------------*/

// Zielsymbol grau darstellen
public ContextTransportTargetno:
  [Transportziel|Image=S37K|Condition=TransportTargetNotSet]
  // Ziel speichern
  return(SetTransportTarget());                                  

public ContextTransportTargetyes:                                
  [Transportziel|Image=S47K|Condition=TransportTargetIsSet]
  // Ziel speichern
  return(SetTransportTarget());

/* Weil man kein Not() um die Condition= schreiben kann */
public TransportTargetNotSet:
  if(Not( TransportTargetIsSet() )) return(1);
  return(0);


/* Overwatch -------------------------------------------------*/

public ContextOverwatch:
  [Überwachungsmodus|Image=SZ7K|Condition=AtLeastOneOverwatchBitAndRouteIsSet]
  SoundConfirm();
  ActivateOverwatch();
  return(1);

AtLeastOneOverwatchBitAndRouteIsSet:

/* Wäre es nicht schön, wenn c4script Operatoren hätte? */

  return(Or(
    And(
      GetBit(0),
      TransportIsPossible()
    ),
    And(
      GetBit(1),
      Or(
        And(
          GetBit(3),
          TransportIsPossible()
        ),
        And(
          Not(GetBit(3)),
          Transport2IsPossible()
        )
      )
    ),
    And(
      And(
        GetBit(2),
        GetBit(3)
      ),
      TransportIsPossible()
    )
  ));



/* Profimodus -------------------------------------------------*/

public ContextAdvancedOptions:
  [Profimodus|Image=S57K]

  // Standardmäßig wird das Overwatchmenu geöffnet
  return(OpenMenuCheck());


/* Profimodus #################################################*/

/* Die Navigationsleiste --------------------------------------*/

CreateNavBar: // Ausgewähltes Item
/*
  if(Not(Equal(Par(),1))

  if(Equal(Par(),1))
*/
  // Overwatch
  if(Par())
    AddMenuItem("Wachmodus","OpenMenuCheck",S87K,this());
  if(Not(Par()))
    AddMenuItem("Wachmodus","OpenMenuCheck",X17K,this());

  // Green ID
  if(Not(Equal(Par(),1)))
    AddMenuItem("Objekttyp","OpenMenuGreenDefineID",S77K,this());
  if(Equal(Par(),1))
    AddMenuItem("Objekttyp","OpenMenuGreenDefineID",X27K,this());

  // Blue Options
  if(Not(Equal(Par(),2)))
    AddMenuItem("Blaue Transportoptionen","OpenMenuBlueOptions",SG7K,this());
  if(Equal(Par(),2))
    AddMenuItem("Blaue Transportoptionen","OpenMenuBlueOptions",S67K,this());

  // Blue ID
  if(Not(Equal(Par(),3)))
    AddMenuItem("Blauer Objekttyp","OpenMenuBlueDefineID",SM7K,this());
  if(Equal(Par(),3))
    AddMenuItem("Blauer Objekttyp","OpenMenuBlueDefineID",SR7K,this());

  // Communication
  if(Not(Equal(Par(),4)))
    AddMenuItem("Kommunikation","OpenMenuCommunication",X37K,this());
  if(Equal(Par(),4))
    AddMenuItem("Kommunikation","OpenMenuCommunication",X47K,this());

  return(1);


/* Normales Menü ----------------------------------------------*/
/*
OpenContext:
  SoundMenuSelect();
  return(SetCommand(this(),"Context",0,0,0,this()));
*/


/* Wachmodus --------------------------------------------------*/

OpenMenuCheck:
  SoundMenuSelect();

  CreateMenu(S87K,this(),this());
  CreateNavBar();
//    AddMenuItem("Zurück","OpenAdvancedOptions",S67K,this());

    if(Not(TransportStartIsObject()))
      And(
        SetVar(0,Local(0)),
        SetVar(2,Local(1))
      );

    if(TransportStartIsObject())
      SetVar(1,GetName(Local()));

    // Grüner Startpunkt
    if(Not(GetBit(0)))
      AddMenuItem(
        ItemString(1),
        "SelectCheckGreen",S97K,this(),GetBit(3)
      );
    if(GetBit(0))
      AddMenuItem(ItemString(1),"SelectCheckGreenNot",SA7K,this(),GetBit(3));

    SetVar();
    SetVar(1);
    SetVar(2);


    // Blauer Startpunkt
    if(And(
      Not(GetBit(1)),
      Not(GetBit(3))
    ))
      AddMenuItem(ItemString(4),"SelectCheckBlueOrGreen2",SB7K,this());
    if(And(
      GetBit(1),
      Not(GetBit(3))
    ))
      AddMenuItem(ItemString(4),"SelectCheckBlueOrGreen2Not",SD7K,this());

    // Grün2
    if(And(
      Not(GetBit(1)),
      GetBit(3)
    ))
      AddMenuItem(ItemString(2),"SelectCheckBlueOrGreen2",S97K,this(),2,0,
        "Dieses Symbol auswählen, damit der Siedlerclonk den grünen Transportstart Nr.2 regelmäßig überprüft."
      );
    if(And(
      GetBit(1),
      GetBit(3)
    ))
      AddMenuItem(ItemString(2),"SelectCheckBlueOrGreen2Not",SA7K,this(),2,0,
        "Der Siedlerclonk überprüft von Zeit zu Zeit, ob sich am grünen Transportstart Nr.2 Objekte befinden und bringt diese gegebenen Falls zum Transportziel."
      );

    // Grün3
    if(And(
      Not(GetBit(2)),
      GetBit(3)
    ))
      AddMenuItem(ItemString(3),"SelectCheckGreen3",S97K,this(),3,0,
        "Dieses Symbol auswählen, damit der Siedlerclonk den grünen Transportstart Nr.3 regelmäßig überprüft.");
    if(And(
      GetBit(2),
      GetBit(3)
    ))
      AddMenuItem(ItemString(3),"SelectCheckGreen3Not",SA7K,this(),3,0,
        "Der Siedlerclonk überprüft von Zeit zu Zeit, ob sich am grünen Transportstart Nr.3 Objekte befinden und bringt diese gegebenen Falls zum Transportziel."
      );

    // Vorauswahl 
//    SelectMenuItem(0); Logischer Weise unnötig

  return(1);


ItemString:
  // Grüner Start

  if(Equal( Par(),1 ))
    if(Not( TransportStartIsSet() ))
      return("Grüner Start");

  if(Equal( Par(),1 ))
    if(And(
      TransportStartIsSet(),
      TransportStartIsObject()
    ))
      return(Format( "Grüner Start (%s)", GetName(Local()) ));

  // zum debuggen:
//  return("test");

  if(Equal(Par(),1))
    if(And(
      TransportStartIsSet(),
      Not(TransportStartIsObject())
    ))
      return(Format("Grüner Start (%d/%d)",Local(),Local(1)));


  // Grüner Start Nr 2

  if(Equal(Par(),2))
    if(Not(TransportStart2IsSet()))
      return("Grüner Start Nr.2");

  if(Equal(Par(),2))
    if(And(
      TransportStart2IsSet(),
      TransportStart2IsObject()
    ))
      return(Format("Grüner Start Nr.2 (%s)",GetName(Local(5))));

  if(Equal(Par(),2))
    if(And(
      TransportStart2IsSet(),
      Not(TransportStart2IsObject())
    ))
      return(Format("Grüner Start Nr.2 (%d/%d)",Local(5),Local(6)));


  // Grüner Start Nr.3

  if(Equal(Par(),3))
    if(Not(TransportTarget2IsSet()))
      return("Grüner Start Nr.3");

  if(Equal(Par(),3))
    if(And(
      TransportTarget2IsSet(),
      TransportTarget2IsObject()
    ))
      return(Format("Grüner Start Nr.3 (%s)",GetName(Local(7))));

  if(Equal(Par(),3))
    if(And(
      TransportTarget2IsSet(),
      Not(TransportTarget2IsObject())
    ))
      return(Format("Grüner Start Nr.3 (%d/%d)",Local(7),Local(8)));


  // Blauer Start

  if(Equal(Par(),4))
    if(Not(TransportStart2IsSet()))
      return("Blauer Start");

  if(Equal(Par(),4))
    if(And(
      TransportStart2IsSet(),
      TransportStart2IsObject()
    ))
      return(Format("Blauer Start (%s)",GetName(Local(5))));

  if(Equal(Par(),4))
    if(And(
      TransportStart2IsSet(),
      Not(TransportStart2IsObject())
    ))
      return(Format("Blauer Start (%d/%d)",Local(5),Local(6)));

  // Fehler (kommt eigentlich nie vor)
  Sound("Error");
  Log("FEHLER IM SIEDLERCLONK");
  return(1);


SelectCheckGreen:
  SoundMenuSelect();
  SetBit(0,1);
  if(AtLeastOneOverwatchBitAndRouteIsSet())
    ActivateOverwatch();
  OpenMenuCheck();
  SelectMenuItem(5);
  return(1);

SelectCheckGreenNot:
  SoundMenuSelect();
  SetBit(0,0);
  OpenMenuCheck();
  SelectMenuItem(5);
  return(1);


SelectCheckBlueOrGreen2:
  SoundMenuSelect();
  SetBit(1,1);
  if(AtLeastOneOverwatchBitAndRouteIsSet())
    ActivateOverwatch();
  OpenMenuCheck();
  SelectMenuItem(6);
  return(1);

SelectCheckBlueOrGreen2Not:
  SoundMenuSelect();
  SetBit(1,0);
  OpenMenuCheck();
  SelectMenuItem(6);
  return(1);

SelectCheckGreen3:
  SoundMenuSelect();
  SetBit(2,1);
  if(AtLeastOneOverwatchBitAndRouteIsSet())
    ActivateOverwatch();
  OpenMenuCheck();
  SelectMenuItem(7);
  return(1);

SelectCheckGreen3Not:
  SoundMenuSelect();
  SetBit(2,0);
  OpenMenuCheck();
  SelectMenuItem(7);
  return(1);


/* ID green----------------------------------------------------*/

OpenMenuGreenDefineID:
  SoundMenuSelect();
  return(OpenMenuGreenDefineID2());

OpenMenuGreenDefineID2:
  CreateMenu(S77K,this(),this());
  CreateNavBar(1);

  // Klammer links
  AddMenuItem(0,"GreenSelectNothing",SP7K);

  // Alles
  if(Not(Local(4)))
    AddMenuItem("Zur Zeit werden alle tragbaren Objekte transportert.","GreenSelectNothing",ST7K);
  // oder nur dieser Typ
  if(And(
    Local(4),
    Not(GetBit(7))
  ))
    AddMenuItem("Nur dieser Objekttyp wird transportiert.","GreenSelectNothing",Local(4));

  if(And(
    Local(4),
    GetBit(7)
  ))
    AddMenuItem("Alles außer diesem Objekttyp wird transportiert.","GreenSelectNothing",Local(4));

  // Klammer rechts
  AddMenuItem(0,"GreenSelectNothing",SQ7K);

  // Automatisch verkaufen
  if(Not(GetBit(5)))
    AddMenuItem("Automatisch verkaufen","GreenSelectAutoSell",X57K);
  if(GetBit(5))
    AddMenuItem("nicht automatisch verkaufen","GreenSelectAutoSellNot",X67K);

  // Alles außer Objekttyp transportieren
  if(Not(GetBit(7)))
    AddMenuItem("Objekttyp ignorieren","GreenSelectIgnore",X77K);
  if(GetBit(7))
    AddMenuItem("Objekttyp ignorieren","GreenSelectIgnoreNot",X87K);

  // Alles transportieren
  AddMenuItem("Alles transportieren","GreenSelectAll",ST7K,this());

  // Vorauswahl
  SelectMenuItem(1);

  // Ein Objekt erstellen, welches nicht den Punktewert verändert
  SetVar(9,CreateObject(S_7K));

  SetVar(0,-1);
  while(SetVar(1,GetDefinition(IncVar(),16)))
    if(ObjectCall(Var(9),"StimmtderOCF",Var(1)))
      AddMenuItem("Nur diesen Objekttyp transportieren: %s","GreenSelectThisID",Var(1));

  // Hilfsobjekt wieder löschen
  RemoveObject(Var(9));

  return(1);


GreenSelectAutoSell:
  SoundMenuSelect();
  SetBit(5,1);
  OpenMenuGreenDefineID();
  SelectMenuItem(8);
  return(1);


GreenSelectAutoSellNot:
  SoundMenuSelect();
  SetBit(5,0);
  OpenMenuGreenDefineID();
  SelectMenuItem(8);
  return(1);


GreenSelectIgnore:
  SoundMenuSelect();
  SetBit(7,1);
  OpenMenuGreenDefineID();
  SelectMenuItem(9);
  return(1);


GreenSelectIgnoreNot:
  SoundMenuSelect();
  SetBit(7,0);
  OpenMenuGreenDefineID();
  SelectMenuItem(9);
  return(1);


GreenSelectAll:
  SoundMenuSelect();
  SetLocal(4);
  OpenMenuGreenDefineID();
  SelectMenuItem(6);
  return(1);


GreenSelectNothing:
  OpenMenuGreenDefineID2();
  if(Equal(Par(),SP7K))
    return(SelectMenuItem(5));
  if(Equal(Par(),SQ7K))
    return(SelectMenuItem(7));
  return(SelectMenuItem(6));


GreenSelectThisID:
  SoundMenuSelect();
  SetLocal(4,Par());
  OpenMenuGreenDefineID();
  SelectMenuItem(6);
  return(1);



/* Blaue Optionen --------------------------------------*/

OpenMenuBlueOptions:
  SoundMenuSelect();

  CreateMenu(SG7K,this(),this());
  CreateNavBar(2);

  // Zurück
//  AddMenuItem("Zurück","OpenAdvancedOptions",S67K,this());


  // Konvertieren
  if(Not(GetBit(3)))
    AddMenuItem("Transportpunkte konvertieren","ConvertBlueToGreen",SN7K,this());
  if(GetBit(3))
    AddMenuItem("Transportpunkte zurücksetzen","ConvertReset",SO7K,this());

  SelectMenuItem(2);

  // Falls der Rest konvertiert ist ist hier Schluss
  if(GetBit(3))
    return(1);

  // Start
  if(Not(TransportStart2IsSet()))
    AddMenuItem("Transportstart festlegen","SetTransportStart2",SH7K,this());
  if(TransportStart2IsSet())
    AddMenuItem("Transportstart festlegen","SetTransportStart2",SI7K,this());

  // Ziel
  if(Not(TransportTarget2IsSet()))
    AddMenuItem("Transportziel festlegen","SetTransportTarget2",SJ7K,this());
  if(TransportTarget2IsSet())
    AddMenuItem("Transportziel festlegen","SetTransportTarget2",SK7K,this());

  // Fortsetzen möglich
  if(Transport2IsPossible())
    AddMenuItem("Transport fortsetzen","BeginTryTransport2",SL7K,this());
  if(BadTransport2Conditions())
    AddMenuItem("Fortsetzen nicht möglich","BadBlueResumeSelect",SF7K,this());

  return(1);

BeginTryTransport2:
  SoundConfirm();
  TryTransport2();
  return(1);

BadBlueResumeSelect:
  SoundCommandFailure();
  Message("Start und Ziel|sind identisch!",this());
  OpenMenuBlueOptions();
  SelectMenuItem(3);
  return(1);


ConvertBlueToGreen:
  SoundMenuSelect();
  SetBit(3,1);
  OpenMenuBlueOptions();
  SelectMenuItem(5);
  return(1);

ConvertReset:
  SoundMenuSelect();
  SetBit(3,0);
  OpenMenuBlueOptions();
  SelectMenuItem(5);
  return(1);



/* ID blue-----------------------------------------------------*/

OpenMenuBlueDefineID:
  SoundMenuSelect();
  return(OpenMenuBlueDefineID2());

OpenMenuBlueDefineID2:
  CreateMenu(SR7K,this(),this());
  CreateNavBar(3);

  // Klammer links
  AddMenuItem(0,"GreenSelectNothing",SP7K);

  // Alles
  if(Not(Local(9)))
    AddMenuItem("Zur Zeit werden alle tragbaren Objekte transportert.","GreenSelectNothing",ST7K);

  if(And(
    Local(9),
    Not(GetBit(8))
  ))
    AddMenuItem("Nur dieser Objekttyp wird transportiert.","BlueSelectNothing",Local(9));

  if(And(
    Local(9),
    GetBit(8)
  ))
    AddMenuItem("Alles außer diesem Objekttyp wird transportiert.","BlueSelectNothing",Local(9));


  // Klammer rechts
  AddMenuItem(0,"GreenSelectNothing",SQ7K);

  // Automatisch verkaufen
  if(Not(GetBit(6)))
    AddMenuItem("Automatisch verkaufen","BlueSelectAutoSell",X57K);
  if(GetBit(6))
    AddMenuItem("nicht mehr automatisch verkaufen","BlueSelectAutoSellNot",X67K);

  // Alles außer Objekttyp transportieren
  if(Not(GetBit(8)))
    AddMenuItem("Objekttyp ignorieren","BlueSelectIgnore",X77K);
  if(GetBit(8))
    AddMenuItem("Objekttyp ignorieren","BlueSelectIgnoreNot",X87K);

  // Alles transportieren
  AddMenuItem("Alles transportieren","BlueSelectAll",ST7K,this());


  // Vorauswahl
  SelectMenuItem(3);

  // Ein Objekt erstellen, welches nicht den Punktewert verändert
  SetVar(9,CreateObject(S_7K));

  SetVar(0,-1);
  while(SetVar(1,GetDefinition(IncVar(),16)))
    if(ObjectCall(Var(9),"StimmtderOCF",Var(1)))
      AddMenuItem("Nur diesen Objekttyp transportieren: %s","BlueSelectThisID",Var(1));

  // Hilfsobjekt wieder löschen
  RemoveObject(Var(9));

  return(1);


BlueSelectAutoSell:
  SoundMenuSelect();
  SetBit(6,1);
  OpenMenuBlueDefineID();
  SelectMenuItem(8);
  return(1);

BlueSelectAutoSellNot:
  SoundMenuSelect();
  SetBit(6,0);
  OpenMenuBlueDefineID();
  SelectMenuItem(8);
  return(1);


BlueSelectIgnore:
  SoundMenuSelect();
  SetBit(8,1);
  OpenMenuBlueDefineID();
  SelectMenuItem(9);
  return(1);


BlueSelectIgnoreNot:
  SoundMenuSelect();
  SetBit(8,0);
  OpenMenuBlueDefineID();
  SelectMenuItem(9);
  return(1);


BlueSelectAll:
  SoundMenuSelect();
  SetLocal(9);
  OpenMenuBlueDefineID();
  SelectMenuItem(6);
  return(1);

BlueSelectNothing:
  OpenMenuBlueDefineID2();
  if(Equal(Par(),SP7K))
    return(SelectMenuItem(5));
  if(Equal(Par(),SQ7K))
    return(SelectMenuItem(7));
  return(SelectMenuItem(6));

BlueSelectThisID:
  SoundMenuSelect();
  SetLocal(9,Par());
  OpenMenuBlueDefineID();
  SelectMenuItem(6);
  return(1);



/* Kommunikation -------------------------------------*/

OpenMenuCommunication: // anderer Siedlerclonk
  SoundMenuSelect();
  CreateMenu(S57K,this(),this(),0,0,0,0,1);
  CreateNavBar(4);

  SelectMenuItem(4);

  while(SetVar(0, FindObject(SC7K, 0,0,0,0, 0,0,0,0, Var()) ))
    if(Equal( GetOwner(Var()), GetOwner() ))
      And(
        AddMenuItem(
          Format("Alle Daten an %s übermitteln",GetName(Var())),
          "SendData",
          SC7K,
          this(),0,
          Var()
        ),
        SetVar(1,1)
      );

  if(Not(Var(1)))
    And(
      SoundCommandFailure(),
      Message("Es gibt keine anderen|Siedlerclonks von|Spieler %s!",this(),GetPlayerName(GetOwner()))
    );

  return(1);


SendData: // Anderer Siedlerclonk
  SoundMenuSelect();

  // Locals anpassen
  SetVar(0,-1);
  while(LessThan( IncVar(), 10 ))
    SetLocal( Var(), Local(Var()), Par(1) );

  // Physical "Float" anpassen
  SetPhysical( "Float", GetPhysical("Float"), 0, Par(1) );

  // Menü wird nicht geschlossen und muss deshalb nicht neu geöffnet werden

  // fertig
  return(1);



/* Objekte Transportieren #####################################*/

ActivateOverwatch:
  SetBit(4,1);
    if(And(
      Not(FindObject(SZ7K,0,0,0,0,0,"Overwatch",this())),
      Not(FindObject(SZ7K,0,0,0,0,0,"Overwatch2",this()))
    ))
      ObjectSetAction(CreateObject(SZ7K),"Overwatch",this(),this());
  return(1);

DeactivateOverwatch:
  SetBit(4,0);
  while(SetVar(0,
    FindObject( SZ7K,0,0,0,0,0,"Overwatch",this() )
  ))
    RemoveObject(Var());
  while(SetVar(0,
    FindObject( SZ7K,0,0,0,0,0,"Overwatch2",this() )
  ))
    RemoveObject(Var());
  return(1);


/* Transportstart überprüfen (TimerCall) ----------------------*/

TimerCallCheckTransportStart:

  // Zeichen, wenn im Overwatch
  if(And(
    Or(
      GetBit(0),
      GetBit(1),
      And(GetBit(2),GetBit(3))
    ),
    GetBit(4)
  ))
    // Falls das Zeichen entfernt wurde wird es wieder erstellt. Sonst passiert nichts.
    ActivateOverwatch();

  // anderes Zeichen, wenn ein Transport im Gange ist
/*  if(And(
    GetCommand(),
    GetBit(4)
  ))
    if(SetVar(0,FindObject(SZ7K,0,0,0,0, 0,"Overwatch",this())),Message("   <->   ",this()))
      RemoveObject(Var());*/

  // Bei geöffnetem Menü, Commands oder direkter Steuerung rennt der Siedler natürlich nicht weg
  if(Or(
    GetMenu(this()),
    GetCommand(),
    Not(GetBit(4))
  ))
    return();

  if(Or(
    Contents(),
    And(
      GetBit(0),
      ObjectAtStart()
    ),
    And(
      GetBit(3),
      Or(
        And(
          GetBit(1),
          ObjectAtStart2()
        ),
        And(
          GetBit(2),
          ObjectAtTarget2()
        )
      )
    )
  ))
    return(1,TryTransport());

  if(And(
    And(
      Or(
        GetBit(1),
        GetBit(9)
      ),
      Not(GetBit(3))
    ),
    And(
      Transport2IsPossible(),
      ObjectAtStart2()
    )
  ))
    return(1,TryTransport2(),SetBit(9,0));

  return();


/* Transport versuchen ----------------------------------------*/

TryTransport:
  Debug("TryTransport");
  if(TransportIsPossible()) 
    return(1,ResumeTransport());
  return(0);

TryTransport2:
  Debug("TryTransport2");
  // Die erste Route hat Priorität, falls im Overwatch (ist so kompliziert wie es aussieht)
  if(And(
    GetBit(4),
    Or(
      And(
        ObjectAtStart(),
        GetBit(0)
      ),
      And(
        GetBit(3),
        Or(
          And(
            ObjectAtStart2(),
            GetBit(1)
          ),
          And(
            ObjectAtTarget2(),
            GetBit(2)
          )
        )
      )
    )
  ))
    return(TryTransport(),SetBit(9,1));

  if(Transport2IsPossible()) 
    return(1,ResumeTransport2());  
  return(0);

TryTransportFailed:
  // Start ist weg
  if(Not( TransportStartIsSet() ))
    return(
      Message("Transportstart|ist weg!",this()),
      SetComDir(0)
    );

  // Ziel ist weg
  if(Not( TransportTargetIsSet() ))
    return(
      Message("Transportziel|ist weg!",this()),
      SetComDir(0)
    );

  // Nochmal versuchem
  TryTransport();

  // fertig
  return(1);

TryTransport2Failed:
  // Start ist weg
  if(Not( TransportStart2IsSet() ))
    return(
      Message("Transportstart|ist weg!",this()),
      SetComDir(0)
    );

  // Ziel ist weg
  if(Not( TransportTarget2IsSet() ))
    return(
      Message("Transportziel|ist weg!",this()),
      SetComDir(0)
    );

  // Nochmal versuchem
  TryTransport2();

  // fertig
  return(1);


/* Transport wieder aufnehmen ---------------------------------*/

ResumeTransport:
  Debug("ResumeTransport");

  // Eventuell verkaufen?
  if(And(
    And(
      Contents(),
      TransportTargetIsObject()
    ),
    And(
      GetBit(5),
      TransportTargetIsBase()
    )
  ))
    if(Or(
      // Objekt hat die richtige ID
      And(
        Equal(GetID(Contents()),Local(4)),
        Not(GetBit(7))
      ),
      // oder nicht die zu ignorierende
      And(
        Not(Equal(GetID(Contents()),Local(4))),
        GetBit(7)
      ),
      // oder die ID ist egal
      Not(Local(4))
    ))
      return(1,
        AddCommand(this(),"Put",Local(2)),
        AppendCommand(this(),"Sell",Local(2),1,0,0,0,GetID(Contents())),
        AppendCommand(this(),"Call",this(),0,0,0,0,"TryTransport")
      );


  // Wenn er was in der Hand hat: in's Zielobjekt stecken
  if(And(
    Contents(),
    TransportTargetIsObject()
  ))
    if(Or(
      // Objekt hat die richtige ID
      And(
        Equal(GetID(Contents()),Local(4)),
        Not(GetBit(7))
      ),
      // oder nicht die zu ignorierende
      And(
        Not(Equal(GetID(Contents()),Local(4))),
        GetBit(7)
      ),
      // oder die ID ist egal
      Not(Local(4))
    ))
      return(1,
        AddCommand(this(),"Put",Local(2)),
        AppendCommand(this(),"Call",this(),0,0,0,0,"TryTransport")
      );

  // Wenn er was in der Hand hat: Beim Ziel ablegen
  if(And(
    Contents(),
    Not(TransportTargetIsObject())
  ))
    if(Or(
      // Objekt hat die richtige ID
      And(
        Equal(GetID(Contents()),Local(4)),
        Not(GetBit(7))
      ),
      // oder nicht die zu ignorierende
      And(
        Not(Equal(GetID(Contents()),Local(4))),
        GetBit(7)
      ),
      // oder die ID ist egal
      Not(Local(4))
    ))
      return(1,
        AddCommand(this(),"Drop",0,Sum(Sub(Local(2),8),Random(17)),Local(3)),
        AppendCommand(this(),"Call",this(),0,0,0,0,"TryTransport")
      );

  // Hat nichts in der Hand: sofort weitermachen
  return(DoTransport());


ResumeTransport2:

  // Eventuell verkaufen?
  if(And(
    And(
      Contents(),
      TransportTarget2IsObject()
    ),
    And(
      GetBit(6),
      TransportTarget2IsBase()
    )
  ))
    if(Or(
      // Objekt hat die richtige ID
      And(
        Equal(GetID(Contents()),Local(9)),
        Not(GetBit(8))
      ),
      // oder nicht die zu ignorierende
      And(
        Not(Equal(GetID(Contents()),Local(9))),
        GetBit(8)
      ),
      // oder die ID ist egal
      Not(Local(9))
    ))
      return(1,
        AddCommand(this(),"Put",Local(7)),
        AppendCommand(this(),"Sell",Local(7),1,0,0,0,GetID(Contents())),
        AppendCommand(this(),"Call",this(),0,0,0,0,"TryTransport2")
      );


  // Wenn er was in der Hand hat: in's Zielobjekt stecken
  if(And(
    Contents(),
    TransportTarget2IsObject()
  ))
    if(Or(
      // Objekt hat die richtige ID
      And(
        Equal(GetID(Contents()),Local(9)),
        Not(GetBit(8))
      ),
      // oder nicht die zu ignorierende
      And(
        Not(Equal(GetID(Contents()),Local(9))),
        GetBit(8)
      ),
      // oder die ID ist egal
      Not(Local(9))
    ))
      return(1,
        AddCommand(this(),"Put",Local(7)),
        AppendCommand(this(),"Call",this(),0,0,0,0,"TryTransport2")
      );

  // Wenn er was in der Hand hat: Beim Ziel ablegen
  if(And(
    Contents(),
    Not(TransportTarget2IsObject())
  ))
    if(Or(
      // Objekt hat die richtige ID
      And(
        Equal(GetID(Contents()),Local(9)),
        Not(GetBit(8))
      ),
      // oder nicht die zu ignorierende
      And(
        Not(Equal(GetID(Contents()),Local(9))),
        GetBit(8)
      ),
      // oder die ID ist egal
      Not(Local(9))
    ))
      return(1,
        AddCommand(this(),"Drop",0,Sum(Sub(Local(7),8),Random(17)),Local(8)),
        AppendCommand(this(),"Call",this(),0,0,0,0,"TryTransport2")
      );

  // Hat nichts in der Hand: sofort weitermachen
  return(DoTransport2());


/* Transport beginnen -----------------------------------------*/

DoTransport:
  // Var(0) Objekt an Start 1
  // Var(1) Ob an S2
  // Var(2) Ob an S3
  // Var(4) Zu transportierendes Obj

  Debug("DoTransport");

  if(StartEqualTarget())
    return(
      1,
      Message("Start und Ziel|sind identisch!",this()),
      SoundCommandFailure()
    );

  // Objekt an einem der Starts speichern
  SetVar(0,ObjectAtStart());
  if(GetBit(3))
    And(
      SetVar(1,ObjectAtStart2()),
      SetVar(2,ObjectAtTarget2())
    );

  SetVar(4,Var(2));
  if(Var(1)) SetVar(4,Var(1));
  if(Var(0)) SetVar(4,Var(0));

  Debug("Var4=%i",Var(4));

  // Eines der eventuell gefundenen Objekte in Var(0) speichern
  SetVar(0,Var(4));

  Debug("DoTransport:Var(0)=%d",Var());

  // Falls keins gefunden wurde wird nichts gemacht
  if(Not(Var()))
    return(TransportFailed());

  Debug("Los geht's!");

  Debug("Verkaufen?");

  if(And(
    TransportTargetIsObject(),
    And(
      GetBit(5),
      TransportTargetIsBase()
    )
  ))
    return(
      1,
      // Objekt ins Ziel legen,
      AddCommand(this(),"Put",Local(2),0,0,Var(0)),
      // Verkaufen
      AppendCommand(this(),"Sell",Local(2),1,0,0,0,GetID(Var())),
      // und nochmal versuchen
      AppendCommand(this(),"Call",this(),0,0,0,0,"TryTransport")
    );

  Debug("Reinlegen");

  if(TransportTargetIsObject())
    return(
      1,
      // Objekt ins Ziel legen,
      AddCommand(this(),"Put",Local(2),0,0,Var(0)),
      // und nochmal versuchen
      AppendCommand(this(),"Call",this(),0,0,0,0,"TryTransport")
    );

  Debug("Hinlegen");
  if(Not( TransportTargetIsObject() ))
    return(
      1,
      // Objekt auf den Boden legen
      AddCommand(this(),"Drop",Var(0),Sum(Sub(Local(2),4),Random(9)),Local(3)),
      // und nochmal versuchen
      AppendCommand(this(),"Call",this(),0,0,0,0,"TryTransport")
    );

  /* Das dürfte eigentlich nie passieren */
  Sound("Eliminated");
  Message("Unerwarteter Fehler|im Siedlerclonk|Bitte informieren Sie mich:|Michael.Zinn@gmx.de");
  Log("Unerwarteter Fehler im Siedlerclonk. Bitte informieren Sie mich: Michael.Zinn@gmx.de");
  return(0);


DoTransport2:
  if(Start2EqualTarget2())
    return(
      1,
      Message("Start und Ziel|sind identisch!",this()),
      SoundCommandFailure()
    );

  // Wenn nichts am Start ist...
  if(Not( SetVar(0,ObjectAtStart2()) ))
    // ...wird nichts gemacht
    return(TransportFailed());

  if(And(
    TransportTarget2IsObject(),
    And(
      GetBit(6),
      TransportTarget2IsBase()
    )
  ))
    return(
      1,
      // Objekt ins Ziel legen,
      AddCommand(this(),"Put",Local(7),0,0,Var(0)),
      // Verkaufen
      AppendCommand(this(),"Sell",Local(7),1,0,0,0,GetID(Var())),
      // und nochmal versuchen
      AppendCommand(this(),"Call",this(),0,0,0,0,"TryTransport2")
    );

  if(TransportTarget2IsObject())
    return(
      1,
      // Objekt ins Ziel legen,
      AddCommand(this(),"Put",Local(7),0,0,Var(0)),
      // und nochmal versuchen
      AppendCommand(this(),"Call",this(),0,0,0,0,"TryTransport2")
    );

  if(Not( TransportTarget2IsObject() ))
    return(
      1,
      // Objekt auf den Boden legen
      AddCommand(this(),"Drop",Var(0),Sum(Sub(Local(7),4),Random(9)),Local(8)),
      // und nochmal versuchen
      AppendCommand(this(),"Call",this(),0,0,0,0,"TryTransport2")
    );

  /* Das dürfte eigentlich nie passieren */
  Sound("Eliminated");
  Message("Unerwarteter Fehler|im Siedlerclonk|Bitte informieren Sie mich:|Michael.Zinn@gmx.de");
  Log("Unerwarteter Fehler im Siedlerclonk. Bitte informieren Sie mich: Michael.Zinn@gmx.de");
  return(0);


/* Keine Objekt mehr da! --------------------------------------*/
TransportFailed:
  SetComDir(0);

  if(And(
    GetBit(4),
    Or(
      GetBit(0),
      GetBit(1),
      GetBit(2)
    )
  ))
    return();

  // "hmmmm...."
  //SoundCommandFailure();

  SoundWorkDone();
  Message("Fertig!",this());
  return(0);


/* Transportstart/ziel speichern ##############################*/

/* Start festlegen --------------------------------------------*/

SetTransportStart:
  // Bestätigen
  SoundConfirm();

  // Clonk ist irgendwo drin...
  if(SetVar( 0,Contained() ))
    And(
      // Start speichern
      SetLocal(0,Var()),
      // Start als Objekt definieren
      SetLocal(1,-1)
    );

  // Wenn er was festhält...
  if(SEqual( GetAction(),"Push" ))
    And(
      // Start speichern
      SetLocal(0,GetActionTarget()),
      // Start als Objekt definieren
      SetLocal(1,-1)
    );

  if(And(
    // Ist nirgens drin
    Not(Contained()),
    // hält nichts fest
    Not(SEqual( GetAction(),"Push" ))
  ))
    And(
      // X-Position speichern
      SetLocal(0,GetX()),
      // Y-Position speichern
      SetLocal(1,GetY())
    );

  // erstmal warten
  AddCommand(this(),"Wait",0,0,0,0,20);
  // Transportieren, wenn möglich
  AppendCommand(this(),"Call",this(),0,0,0,0,"TryTransport");



  // fertig!
  return(1);


SetTransportStart2:
  // Bestätigen
  SoundConfirm();

  // Clonk ist irgendwo drin...
  if(SetVar( 0,Contained() ))
    And(
      // Start speichern
      SetLocal(5,Var()),
      // Start als Objekt definieren
      SetLocal(6,-1)
    );

  // Wenn er was festhält...
  if(SEqual( GetAction(),"Push" ))
    And(
      // Start speichern
      SetLocal(5,GetActionTarget()),
      // Start als Objekt definieren
      SetLocal(6,-1)
    );

  if(And(
    // Ist nirgens drin
    Not(Contained()),
    // hält nichts fest
    Not(SEqual( GetAction(),"Push" ))
  ))
    And(
      // X-Position speichern
      SetLocal(5,GetX()),
      // Y-Position speichern
      SetLocal(6,GetY())
    );

  // erstmal warten
  AddCommand(this(),"Wait",0,0,0,0,20);
  // Transportieren, wenn möglich
  if(Not(GetBit(3)))
    AppendCommand(this(),"Call",this(),0,0,0,0,"TryTransport2");
  if(GetBit(3))
    AppendCommand(this(),"Call",this(),0,0,0,0,"TryTransport");

  // fertig!
  return(1);

/* Ziel festlegen ---------------------------------------------*/

SetTransportTarget:
  SoundConfirm();
  if(SetVar( 0, Contained() ))      // Clonk ist irgendwo drin...
    And(
      SetLocal(2,Var(0)),           // Ziel speichern
      SetLocal(3,-1)                // Ziel als Objekt definieren
    );

  if(SEqual( GetAction(),"Push" ))        // Wenn er was festhält...
    And(
      SetLocal(2,GetActionTarget()),      // Ziel speichern
      SetLocal(3,-1)                      // Ziel als Objekt definieren
    );

  if(And(
    Not(Contained()),                      // Wenn er im Freien ist...
    Not(SEqual( GetAction(),"Push" ))      // ...und er nichts festhält
  ))
    And(
      SetLocal(2,GetX()),                  // X-Position speichern
      SetLocal(3,GetY())                   // Y-Position speichern
    );

  AddCommand(this(),"Wait",0,0,0,0,20);
  AppendCommand(this(),"Call",this(),0,0,0,0,"TryTransport");

  return(1);


SetTransportTarget2:
  SoundConfirm();
  if(SetVar( 0, Contained() ))      // Clonk ist irgendwo drin...
    And(
      SetLocal(7,Var(0)),           // Ziel speichern
      SetLocal(8,-1)                // Ziel als Objekt definieren
    );

  if(SEqual( GetAction(),"Push" ))        // Wenn er was festhält...
    And(
      SetLocal(7,GetActionTarget()),      // Ziel speichern
      SetLocal(8,-1)                      // Ziel als Objekt definieren
    );

  if(And(
    Not(Contained()),                      // Wenn er im Freien ist...
    Not(SEqual( GetAction(),"Push" ))      // ...und er nichts festhält
  ))
    And(
      SetLocal(7,GetX()),                  // X-Position speichern
      SetLocal(8,GetY())                   // Y-Position speichern
    );

  AddCommand(this(),"Wait",0,0,0,0,20);
  if(Not(GetBit(3)))
    AppendCommand(this(),"Call",this(),0,0,0,0,"TryTransport2");
  if(GetBit(3))
    AppendCommand(this(),"Call",this(),0,0,0,0,"TryTransport");

  return(1);



/* Spezielle Überprüfungen ####################################*/

/* Ist ein Transport möglich? ---------------------------------*/
TransportIsPossible:
  if(And(
    Or(
      TransportStartIsSet(),
      And(
        GetBit(3),
        Or(
          TransportStart2IsSet(),
          TransportTarget2IsSet()
        )
      )
    ),
    And(
      TransportTargetIsSet(),
      Not(StartEqualTarget())
    )
  ))
    return(1);
  return(0);


Transport2IsPossible:
  if(And(
    And(
      TransportStart2IsSet(),
      Not(GetBit(3))
    ),
    And(
      TransportTarget2IsSet(),
      Not(Start2EqualTarget2())
    )
  ))
    return(1);
  return(0);



/* Gibt es einen Start? ---------------------------------------*/
public TransportStartIsSet:
  if(Or(
    TransportStartIsObject(),
    And(
      GreaterThan(Local(0),0),
      GreaterThan(Local(1),0)
    )
  ))
    return(1);
  return(0);

public TransportStart2IsSet:
  if(Or(
    TransportStart2IsObject(),
    And(
      GreaterThan(Local(5),0),
      GreaterThan(Local(6),0)
    )
  ))
    return(1);
  return(0);


/* Gibt es ein Ziel? ------------------------------------------*/
public TransportTargetIsSet:
  if(Or(
    TransportTargetIsObject(),
    And(
      GreaterThan(Local(2),0),
      GreaterThan(Local(3),0)
    )
  ))
    return(1);
  return(0);

public TransportTarget2IsSet:
  if(Or(
    TransportTarget2IsObject(),
    And(
      GreaterThan(Local(7),0),
      GreaterThan(Local(8),0)
    )
  ))
    return(1);
  return(0);

/* Ist der Transportstart ein Objekt? -------------------------*/

TransportStartIsObject:
  if(And(
    Local(0),
    Equal(Local(1),-1)
  ))
    return(1);
  return(0);

TransportStart2IsObject:
  if(And(
    Local(5),               // Ist noch da
    Equal(Local(6),-1)      // Ist als Objekt definiert
  ))
    return(1);
  return(0);

/* Ist das Transportziel ein Objekt? --------------------------*/

TransportTargetIsObject:
  if(And(
    Local(2),               // Ist noch da
    Equal(Local(3),-1)      // Ist als Objekt definiert
  ))
    return(1);
  return(0);

TransportTarget2IsObject:
  if(And(
    Local(7),               // Ist noch da
    Equal(Local(8),-1)      // Ist als Objekt definiert
  ))
    return(1);
  return(0);


/* Ist das Transportziel eine eigene Basis? -------------------*/

TransportTargetIsBase:
  // Wenn das Ziel kein Objekt ist ist es natürlich auch keine Basis
  if(Not( TransportTargetIsObject() ))
    return(0);

  if(Equal( GetBase(Local(2)), GetOwner() ))
    return(1);

  return(0);

TransportTarget2IsBase:
  // Besitzerlose Siedler habe keine Basis
  if(Equal( GetOwner(), -1 ))
    return();

  // Wenn das Ziel kein Objekt ist ist es natürlich auch keine Basis
  if(Not( TransportTarget2IsObject() ))
    return(0);

  if(Equal( GetBase(Local(7)), GetOwner() ))
    return(1);

  return(0);


/* Start ist gleich Ziel (natürlich den Transport abbrechen) --*/

StartEqualTarget:

  if(And(
    TransportStartIsObject(),
    TransportTargetIsObject()
  ))
    if(And(
      Equal(Local(0),Local(2)),
      Equal(Local(1),Local(3))
    ))
      return(1);

  if(And(
    Not(TransportStartIsObject()),
    Not(TransportTargetIsObject())
  ))
    if(And(
      LessThan( Abs(Sub(Local(0),Local(2))),StartAreaWidth() ),
      LessThan( Abs(Sub(Local(1),Local(3))),StartAreaHeight() )
    ))
      return(1);

  return(0);


Start2EqualTarget2:

  if(And(
    TransportStart2IsObject(),
    TransportTarget2IsObject()
  ))
    if(And(
      Equal(Local(5),Local(7)),
      Equal(Local(6),Local(8))
    ))
      return(1);

  if(And(
    Not(TransportStart2IsObject()),
    Not(TransportTarget2IsObject())
  ))
    if(And(
      LessThan( Abs(Sub(Local(5),Local(7))),StartAreaWidth() ),
      LessThan( Abs(Sub(Local(6),Local(8))),StartAreaHeight() )
    ))
      return(1);

  return(0);

Target2EqualTarget:

  if(And(
    TransportTarget2IsObject(),
    TransportTargetIsObject()
  ))
    if(And(
      Equal(Local(7),Local(2)),
      Equal(Local(8),Local(3))
    ))
      return(1);

  if(And(
    Not(TransportTarget2IsObject()),
    Not(TransportTargetIsObject())
  ))
    if(And(
      LessThan( Abs(Sub(Local(7),Local(2))),StartAreaWidth() ),
      LessThan( Abs(Sub(Local(8),Local(3))),StartAreaHeight() )
    ))
      return(1);

  return(0);



/* Ein Objekt am Start finden ---------------------------------*/

ObjectAtStart:
  /* kein Start vorhanden? */
  if(Not( TransportStartIsSet() )) return();

  /* In Objekt */

  if(Not(GetBit(7)))
    while(And(
      TransportStartIsObject(),
      SetVar(0,FindObject( Local(4), 0,0,0,0, OCF_Collectible(),0,0, Local(0), Var() ))
    ))
      if(BitAnd(
        GetOCF(Var()),
        OCF_Fullcon()
      ))
        SetVar(1,Var());

  SetVar();  

  if(GetBit(7))
    while(And(
      TransportStartIsObject(),
      SetVar(0,FindObject( 0, 0,0,0,0, OCF_Collectible(),0,0, Local(0), Var() ))
    ))
      if(Not(Equal( GetID(Var()), Local(4) )))
        if(BitAnd(
          GetOCF(Var()),
          OCF_Fullcon()
        ))
          SetVar(1,Var());

  if(Var(1)) return(Var(1));

  SetVar();

  /* Im Freien */

  if(Not( GetBit(7) ))
   while(
    SetVar(0,FindObject(
      Local(4),
      Sub( Sum( Local(0), Div(StartAreaWidth(), -2)), GetX() ),
      Sub( Sum( Local(1), Div(StartAreaHeight(),-2)), GetY() ),
      StartAreaWidth(),
      StartAreaHeight(),
      OCF_Collectible(),0,0,
      NoContainer(),
      Var()
    ))
   )
    if(And(
      BitAnd( GetOCF(Var()), OCF_InFree() ),
 And( BitAnd( GetOCF(Var()), OCF_Available() ),
      BitAnd( GetOCF(Var()), OCF_Fullcon() )
 )  ))  
      SetVar(1,Var());

  SetVar();

  if(GetBit(7))
   while(
    SetVar(0,FindObject(
      0,
      Sub( Sum( Local(0), Div(StartAreaWidth(), -2)), GetX() ),
      Sub( Sum( Local(1), Div(StartAreaHeight(),-2)), GetY() ),
      StartAreaWidth(),
      StartAreaHeight(),
      OCF_Collectible(),0,0,
      NoContainer(),
      Var()
    ))
   )
    if(And(
      BitAnd( GetOCF(Var()), OCF_InFree() ),
 And( BitAnd( GetOCF(Var()), OCF_Available() ),
      BitAnd( GetOCF(Var()), OCF_Fullcon() )
 )  ))  
      if(Not(Equal( GetID(Var()), Local(4) )))
        SetVar(1,Var());

  return(Var(1));


/* Am blauen Start */
ObjectAtStart2:
  /* kein Start vorhanden? */
  if(Not( TransportStart2IsSet() )) return();

  SetVar(9,Local(9));
  SetVar(8,GetBit(8));
  if(GetBit(3))
    And(
      SetVar(9,Local(4)),
      SetVar(8,GetBit(7))
    );


  /* In Objekt */
  if(Not(Var(8)))
    while(And(
      TransportStart2IsObject(),
      SetVar(0,FindObject( Var(9), 0,0,0,0, OCF_Collectible(),0,0, Local(5), Var() ))
    ))
      if(BitAnd(
        GetOCF(Var()),
        OCF_Fullcon()
      ))
        SetVar(1,Var());

  SetVar();

  if(Var(8))
    while(And(
      TransportStart2IsObject(),
      SetVar(0,FindObject( 0, 0,0,0,0, OCF_Collectible(),0,0, Local(5), Var() ))
    ))
      if(Not(Equal( GetID(Var()), Var(9) )))
        if(BitAnd(
          GetOCF(Var()),
          OCF_Fullcon()
        ))
          SetVar(1,Var());

  if(Var(1)) return(Var(1));

  SetVar();

  /* Im Freien */
  if(Not(Var(8)))
   while(
    SetVar(0,FindObject(
      Var(9),
      Sub( Sum( Local(5), Div(StartAreaWidth(), -2)), GetX() ),
      Sub( Sum( Local(6), Div(StartAreaHeight(),-2)), GetY() ),
      StartAreaWidth(),
      StartAreaHeight(),
      OCF_Collectible(),0,0,
      NoContainer(),
      Var()
    ))
   )
    if(And(
      BitAnd( GetOCF(Var()), OCF_InFree() ),
 And( BitAnd( GetOCF(Var()), OCF_Available() ),
      BitAnd( GetOCF(Var()), OCF_Fullcon() )
 )  ))  
      SetVar(1,Var());

  SetVar();

  /* Im Freien */
  if(Var(8))
   while(
    SetVar(0,FindObject(
      0,
      Sub( Sum( Local(5), Div(StartAreaWidth(), -2)), GetX() ),
      Sub( Sum( Local(6), Div(StartAreaHeight(),-2)), GetY() ),
      StartAreaWidth(),
      StartAreaHeight(),
      OCF_Collectible(),0,0,
      NoContainer(),
      Var()
    ))
   )
    if(And(
      BitAnd( GetOCF(Var()), OCF_InFree() ),
 And( BitAnd( GetOCF(Var()), OCF_Available() ),
      BitAnd( GetOCF(Var()), OCF_Fullcon() )
 )  ))  
      if(Not(Equal( GetID(Var()), Var(9) )))
        SetVar(1,Var());

  return(Var(1));


/* Bei konvertierter blauer Route */
ObjectAtTarget2:
  Debug("ObjectAtTarget2");

  /* kein Ziel vorhanden? */
  if(Not( TransportTarget2IsSet() )) return();

  Debug("Ein Objekt bei T2 suchen");

  /* In Objekt */
  if(Not(GetBit(7)))
  while(And(
    TransportTarget2IsObject(),
    SetVar(0,FindObject( Local(4), 0,0,0,0, OCF_Collectible(),0,0, Local(7), Var() ))
  ))
    if(BitAnd(
      GetOCF(Var()),
      OCF_Fullcon()
    ))
      SetVar(1,Var());

  SetVar();

  if(GetBit(7))
  while(And(
    TransportTarget2IsObject(),
    SetVar(0,FindObject( 0, 0,0,0,0, OCF_Collectible(),0,0, Local(7), Var() ))
  ))
    if(Not(Equal( GetID(Var()), Local(4) )))
      if(BitAnd(
        GetOCF(Var()),
        OCF_Fullcon()
      ))
        SetVar(1,Var());

  Debug("Falls gefunden: liefern");
  if(Var(1)) return(Var(1));

  Debug("Nichts im Obj gefunen, also im freien suchen");


  SetVar();
  SetVar(1);

  /* Im Freien */
 if(Not(GetBit(7)))
  while(
    SetVar(0,FindObject(
      Local(4),
      Sub( Sum( Local(7), Div(StartAreaWidth(), -2)), GetX() ),
      Sub( Sum( Local(8), Div(StartAreaHeight(),-2)), GetY() ),
      StartAreaWidth(),
      StartAreaHeight(),
      OCF_Collectible(),0,0,
      NoContainer(),
      Var()
    ))
  )
    if(And(
      BitAnd( GetOCF(Var()), OCF_InFree() ),
 And( BitAnd( GetOCF(Var()), OCF_Available() ),
      BitAnd( GetOCF(Var()), OCF_Fullcon() )
 )  ))  
      SetVar(1,Var());

SetVar();

 if(GetBit(7))
  while(
    SetVar(0,FindObject(
      0,
      Sub( Sum( Local(7), Div(StartAreaWidth(), -2)), GetX() ),
      Sub( Sum( Local(8), Div(StartAreaHeight(),-2)), GetY() ),
      StartAreaWidth(),
      StartAreaHeight(),
      OCF_Collectible(),0,0,
      NoContainer(),
      Var()
    ))
  )
   if(Not(Equal( GetID(Var()), Local(4) )))
    if(And(
      BitAnd( GetOCF(Var()), OCF_InFree() ),
 And( BitAnd( GetOCF(Var()), OCF_Available() ),
      BitAnd( GetOCF(Var()), OCF_Fullcon() )
 )  ))  
      SetVar(1,Var());


  return(Var(1));



/* Engine-Calls ###############################################*/

/* Spezial2 (überladen) */

protected ControlSpecial2:
  [Menü]
//  SetBit(4,0);
  // Kontext (immer)
  SetCommand(this(),"Context",0,0,0,this());
  // Stehenbleiben
  SetComDir();
  return(1);

/* Automatisches überprüfen stoppen */
protected ControlLeft: DeactivateOverwatch(); SetBit(9); return(0);
protected ControlRight:DeactivateOverwatch(); SetBit(9); return(0);
protected ControlUp:   DeactivateOverwatch(); SetBit(9); return(0);
protected ControlDown: DeactivateOverwatch(); SetBit(9); return(0);
protected ControlDig:  DeactivateOverwatch(); SetBit(9); return(0);
protected ControlThrow:DeactivateOverwatch(); SetBit(9); return(0);


//###############################################################
// Benötigte Baumaterialien selbst herstellen ###################
//###############################################################

/* Wenn das Objekt noch Materialien braucht -------------------*/
ControlCommandFinished: // szCommand, Baustelle
// Variablen:
// Var(0) : Gefundenes Objekt
// Var(1) : Herzustellendes Objekt
// Var(2) : Produktionsstätte

  // nur wenn er was bauen wollte
  if(Not(
    SEqual( Par(0),"Build" )
  ))
    return(0);

  Debug("Component Herausfinden");
  if(Not(SetVar(1, GetFirstMissingComponent(Par(1)) )))
    return(0);

  Debug("Irgendeine Produktionsstätte finden...");
  while(SetVar(0,FindObject(0,
    -1000,-500,2000,1000,0,0,0,0,Var()
  )))
    if(ObjectCall( Var(),"IsProducerOf",this(),Var(1) ))
      SetVar(2,Var(),Debug("Eine Prod. wurde gefunden"));

  SetVar(0,0);

  Debug("Davon die nächstgelegene finden.");
  while(And(
    Var(2),
    SetVar(0,FindObject( 0, -500,-250,1000,500, 0,0,0,0, Var() ))
  ))
    if(ObjectCall( Var(),"IsProducerOf",this(),Var(1) ))
      if(LessThan( ObjectDistance(Var()), ObjectDistance(Var(2)) ))
        SetVar(2,Var(0));

  if(Var(2))
    And(
      Debug("Herstellung wird eingeleitet..."),
      // Meldung unterdrücken
      Message("",this()),
      // Selbige natürlich erst herstellen
      AddCommand(this(),"Call",Var(2),Var(1),0,0,0,"HowToProduce"),
      // Aber zuerst die Baumaterialien nehmen
      AppendCommand(this(),"Acquire",0,0,0,0,0,Var(1),5),
      // Weiter bauen
      AppendCommand(this(),"Build",Par(1))
    );

  Debug("Fertig!");
  Debug("---------------");
  return();