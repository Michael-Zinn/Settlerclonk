/*-- Hilfsobjekt --*/

/* Wird in jedem Fall überladen (wegen #include). Das hier verhindert nur sinnlose Fehlermeldungen.
Würden diese functions nicht in diesem Script stehen, würde es nicht funktionieren. Das ist wahrscheinlich ein Fehler in der GWE2.*/
Production:return();
SelectProduction:return();
FindIncompleteContents:return();

/* Umleitung */
ProductTyp:
  if(Or(
    Equal(GetID(),CHEM),
    Equal(GetID(),WRKS)
  ))
    return(Call(Format("%iProductTyp",GetID()),Par(),Par(1)));
  return();

ProductCondition:
  if(Or(
    Equal(GetID(),CHEM),
    Equal(GetID(),WRKS)
  ))
    return(Call(Format("%iProductCondition",GetID()),Par(),Par(1)));
  return();

ProductCall:
  if(Or(
    Equal(GetID(),CHEM),
    Equal(GetID(),WRKS)
  ))
    return(Call(Format("%iProductCall",GetID()),Par(),Par(1)));
  return();

IsProducerOf:
  Debug("IsProducerOf");
  if(Or(
    Equal(GetID(),CHEM),
    Equal(GetID(),WRKS),
    Equal(GetID(),FNDR),
    Equal(GetID(),SAWM)
  ))
    return(Call(Format("%iIsProducerOf",GetID()),Par(),Par(1)));
  return();

HowToProduce:
  Debug("HowToProduce");
  if(Or(
    Equal(GetID(),CHEM),
    Equal(GetID(),WRKS),
    Equal(GetID(),FNDR),
    Equal(GetID(),SAWM)
  ))
    return(Call(Format("%iHowToProduce",GetID()),Par(),Par(1)));
  return();


/* CHEMIEFABRIK ################################################*/

CHEMProductTyp:return(C4D_Object());
CHEMProductCondition:return(1);
CHEMProductCall:return("IsChemicalProduct");

/* Umleitung */
CHEMIsProducerOf:
  return(WRKSIsProducerOf(Par(),Par(1)));

CHEMHowToProduce:
  return(WRKSHowToProduce(Par(),Par(1)));


/* WERKSTATT ###################################################*/

/* Erforderliche Produkteigenschaften (überladbar) */
WRKSProductTyp:return(C4D_Vehicle());
WRKSProductCondition:return();
WRKSProductCall:return();

/* Produktionseigenschaften */
WRKSIsProducerOf: // Frager, Objekt-ID
  Debug("WRKSIsProducerOf");
  if(ProductCondition())
    if(Not(DefinitionCall( Par(1),ProductCall() )))
      return();

  Debug("Fertigstellung prüfen");
  // Ist es fertig gebaut?
  if(Not(
    BitAnd(GetOCF(),OCF_Fullcon())
  ))
    return();

  Debug("Eigenschaften überprüfen");
  // Kann nicht hergestellt werden oder kein Plan:
  if(Not(And(
    BitAnd( GetCategory(0,Par(1)), ProductTyp() ),
    GetPlrKnowledge( GetOwner(Par()),Par(1) )
  )))
    return();

  Debug("Suche nach besserer...");
  if(And(
    NeedsPowerSupply(),
    FindSuppliedObjectCloseTo(Par())
  ))
    return(0,Debug("...bessere Werkstattgefunden"));
  Debug("...diese Werkstatt ist perfekt! Los geht's.");
  return(1);


/* Wie man etwas herstellt */
WRKSHowToProduce: // Frager, Objekt-ID
  if(NeedsPowerSupply())
    return(
      1,
      Debug("Versorgen, dann aufrufen"),
      AddCommand(
        Par(0),
        "Call",
        this(),
        Par(1),
        0,0,0,
        "HowToProduce"
      ),
      AddCommand(Par(0),"Energy",this()),
      Debug("Fertig")
    );

  AddCommand(Par(),"Wait",0,0,0,0, 0, 10);
  SelectProduction2( Par(1),Par(0) ); // Keine KI-Befehle löschen
  return(1);

/* Produktion */

/* Keine Commands löschen (AddCommand statt SetCommand)*/
public SelectProduction2: // C4ID idType, C4Object *pWorker
  // ...und mit der Arbeit beginnen
  AddCommand(Par(1),"Call",this(),Par(0),0,0,0,"StartProduction2");
  // Arbeiter soll die Werkstatt betreten...
  AddCommand(Par(1),"Enter",this());
  return(1);
  
  
public StartProduction2: // C4Object *pWorker, C4ID idType
  // Nach unfertigem Objekt zum Weiterbauen suchen
  SetVar(0,FindIncompleteContents(Par(1)));
  // Sonst neues Objekt erzeugen
  if (Not(Var(0)))
    if (Not(SetVar(0, CreateConstruction(Par(1),0,0,0, 1) ))) return(0);
  // Erzeugtes Objekt in die Werkstatt versetzen
  if (Not(Equal( Contained(Var(0)),this() ))) 
    Enter(this(),Var(0));
  // Baubefehl
  AddCommand(Par(0),"Build",Var(0));
  return(1);
  
/* HOCHOFEN ##################################################*/

/* Produktionsstätte */
FNDRIsProducerOf:
  if(And(
    Equal(Par(1),METL),
    BitAnd(GetOCF(),OCF_Fullcon())
  ))
    return(1);
  return();

FNDRHowToProduce:
  AddCommand(Par(),"Wait",0,0,0,0, 0, 185);
  Production(Par(),METL);
  return(1);

/* SÄGEWERK ##################################################*/


// Herstellungseigenschaften ##############################

SAWMIsProducerOf: // Frager,ID
  if(Not(Equal(GetID(),SAWM))) return(IsProducerOf());
  Debug("IsProducerOf");

  if(GameCall("Debugmode"))
    Message("[%i]: %s",this(),GetID(),"Ich wurde|gefragt!|I|\I/");

  if(Not(
    Equal(Par(1),WOOD)
  ))
    return(0,Debug("Es wurde nicht nach WOOD gefragt"));

  if(Not(
    BitAnd(GetOCF(this()),OCF_Fullcon())
  ))
    return(0);

  Debug("Jetzt wird ein besseres gesucht...");
  if(And(
    NeedsPowerSupply(this()),
    FindSuppliedObjectCloseTo(Par())
  ))
    return(0,Debug("Es gibt ein besseres Sägewerk"));
  Debug("Einer Produktion steht nichts mehr im Wege");
  return(1);

// Kann Holz produzieren
SAWMHowToProduce: // Frager, Objekttyp
  if(Not(Equal(GetID(),SAWM))) return(HowToProduce());

  Debug("HowToProduce:");

  if(GameCall("Debugmode"))
    Message("[%i]: %s",this(),GetID(),"Ich werde|Holz herstellen!|I|\I/");

  if(Not(
    BitAnd(GetOCF(),OCF_Fullcon())
  ))
    return(0);

  if(Equal(Par(1),WOOD))
    if(And(
      NeedsPowerSupply(),
      FindSuppliedObjectCloseTo(Par())
    ))
      return(0,Debug("Es gibt bessere SAWMs"));

  Debug("Eventuell erst versorgen");
  if(NeedsPowerSupply())
    return(
      1,
      Debug("Versorgen, dann aufrufen"),
      AddCommand(
        Par(0),
        "Call",
        this(),
        WOOD,
        0,0,0,
        "HowToProduce"
      ),
      AddCommand(Par(0),"Energy",this()),
      Debug("Fertig")
    );


  Debug("Warten, bis das Holz fertig ist.");
  AddCommand(Par(0),"Wait",0,0,0,0,0,70);

  Debug("... aber erst einen Baum verarbeiten");
  AddCommand(Par(0),"Call",this(),0,0,0,0,"StartProduction");

  return(1);

/* ENDE DER PRODUKTIONSSTÄTTEN ########################################*/

/* BA7K: (gekürzt) ####################################################*/


GetFirstMissingComponent: // pObj
  if(Not(
    SetVar(0,Par())
  ))
    SetVar(0,this());
  SetVar(1,-1);
  while(SetVar(2, GetComponent( 0,IncVar(1),0,GetID(Var(0)) ) ))
    if(LessThan(
      GetComponent(Var(2),0,Var()),
      GetComponent(Var(2),0,0,GetID(Var()))
    ))
      return(Var(2));

  // nichts wird benötigt
  return(0);

NeedsPowerSupply: // C4Object *pObject
  if(Not(
    SetVar(0,Par())
  ))
    SetVar(0,this());

  if(And(
    FindObject(ENRG),
    And(
      BitAnd( GetOCF(Var(0)),OCF_PowerConsumer() ),
      Not(FindObject( PWRL, 0,0,0,0, 0,"Connect",Var(0) ))
    )
  ))
    return(1);

  return(0);

Debug:
  if(GameCall("Debugmode"))
    return(Log("[%i]: %s",GetID(),Format(
      Par(0),Par(1),Par(2),Par(3),Par(4),Par(5),Par(6),Par(7),Par(8),Par(9)
    )));
  return(0);

FindSuppliedObjectCloseTo: // Objekt, ID
  if(Not(SetVar( 1, Par(1) )))
    SetVar(1,GetID());

  Debug("Suche %i bei %s",Var(1),GetName(Par()));

  while(
    SetVar(0,
      FindObject(Var(1),
Sub(    Sum(GetX(Par()),-1000),    GetX()),
Sub(    Sum(GetY(Par()),-500),     GetY()),
        2000,
        1000,
        OCF_Fullcon(),0,0,0,
        Var()
      )
    )
  )
    if(Not(
      NeedsPowerSupply( Var(0) )
    ))
      return(1);
  return(0);


/* Dieses Objekt stellt die Grundfunktionalität für Gebäude
   mit Tür zur Verfügung. Die Aktionen OpenDoor, DoorOpen
   und CloseDoor müssen selbstverständlich in der ActMap der
   jeweiligen Gebäude vorhanden sein. */

protected ActivateEntrance: 
  if (ActIdle()) SetAction("OpenDoor");
  return(1);
  
private OpenEntrance:
  SetEntrance(1);
  return(1);

private CloseEntrance:
  SetEntrance(0);
  return(1);

private SoundOpenDoor:
  return(Sound("DoorOpen"));

private SoundCloseDoor:
  return(Sound("DoorClose"));
  
private DoorClosed:
  return(1);