/* Par(0)^Par(1) */
Pow:
  if(LessThan( Par(1),0 )) IncVar(2);
  IncVar();
  while(LessThan( Var(1),Abs(Par(1)) ))
    And(
      SetVar(0,Mul( Var(),Par() )),
      IncVar(1)
    );
  if(Var(2)) return(Div( 1,Var() ));
  return(Var());



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

SetCon: // Menge, Objekt
  if(Not(
    SetVar(0,Par())
  ))
    SetVar(0,this());
  return(DoCon( Sub( Par(0), GetCon(Var()) ), Var() ));

IncLocal: // int iVarIndex, C4Object *pObj
  return(SetLocal(
    Par(),
    Sum(Local(Par(),Var()),1),
    Var()
  ));

DecLocal: // int iVarIndex, C4Object *pObj
  return(SetLocal(
    Par(),
    Sum(Local(Par(),Var()),-1),
    Var()
  ));

IncGlobal: // int iVarIndex
  return(SetGlobal(
    Par(),
    Sum(Global(Par()),1)
  ));

DecGlobal: // int iVarIndex
  return(SetGlobal(
    Par(),
    Sum(Global(Par()),-1)
  ));

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


/*HasBase:
  if(Not(
    SetVar(0,Par())
  ))
    SetVar(0,this());
  return(And(
    FindBase(SetVar(1,GetOwner(Var()))),
    Not(Equal( GetBase(Contained(Var())), Var(1) )) 
  ));
*/
/*
DrawMaterialRect: // iMaterial, iX, iY, iWidth, iHeight
  return(DrawVolcanoBranch(
    Par(0),
    SetVar(0,Sum( Par(1),Div(Par(3),2) )),
    Par(2),
    Var(0),
    Sub( Par(2),Par(4) ),
    Par(3)
  ));
*/