/*-- Overwatch zeigen --*/

protected Initialize:
  SetAlive(0);
  return(1);

private Check:
  // Vertex anpassen
  AdaptVertex();

  // Ziel ist nicht im Overwatch mode
  if(Not(
    And(
      BitAnd( GetPhysical("Float",0,GetActionTarget(1)), 16),
      Or(
        BitAnd( GetPhysical("Float",0,GetActionTarget(1)), 1),
        BitAnd( GetPhysical("Float",0,GetActionTarget(1)), 2),
        And(
          BitAnd( GetPhysical("Float",0,GetActionTarget(1)), 4),
          BitAnd( GetPhysical("Float",0,GetActionTarget(1)), 8)
        )
      )
    )
  ))
    return(RemoveObject());

  // Zielobjekt verschwunden
  if (Not(GetActionTarget(1))) return(RemoveObject());  

  // Ziel gestorben
  if (Not(GetAlive(GetActionTarget(1)))) return(RemoveObject());

  // Ziel in Behälter
  if (Contained(GetActionTarget(1)))
    if (Not(Equal( GetActionTarget(0), Contained(GetActionTarget(1)) )))
      return(SetAction("Overwatch",Contained(GetActionTarget(1)),GetActionTarget(1)));

  // Ziel im Freien
  if (Not(Contained(GetActionTarget(1))))
    if (Not(Equal( GetActionTarget(0), GetActionTarget(1) )))
      return(SetAction("Overwatch",GetActionTarget(1),GetActionTarget(1)));      

  return(1);


/* Passt den Vertex so an, dass auch wenn der Siedler in einem Gebäude ist, das Auge über ihm erscheint.*/

AdaptVertex:

  // Normal
  SetVertex();
  SetVertex(0,1,11);

  // Eventuell an das Gebäude anpassen
  if(SetVar(0,Contained(SetVar(1,GetActionTarget(1)))))
    And(
      SetVertex(0,0,
        Sub(
          Sum(GetX(Var(0)),GetVertex(0,0,Var(0))),
          Sum(GetX(Var(1)),GetVertex(0,0,Var(1)))
        )
      ),
      SetVertex(0,1,
        Sum(
          Sub(
            Sum(GetY(Var(0)),GetVertex(0,1,Var(0))),
            Sum(GetY(Var(1)),GetVertex(0,1,Var(1)))
          ),
          11
        )
      )
    );

  SetVar(2,this());

  if(Not(Var())) return(1);

  // Anpassen
  while(SetVar(2, FindObject(SZ7K, 0,0,0,0, 0,"Overwatch",Var(),0, Var(2)) ))
    // Vertex anpassen
    SetVertex(0,0,Sum(GetVertex(),4));

  SetVar(2,this());

  // Anpassen
  while(SetVar(2, FindObject(SZ7K, 0,0,0,0, 0,"Overwatch2",Var(),0, Var(2)) ))
    // Vertex anpassen
    SetVertex(0,0,Sum(GetVertex(),4));


  // Erstmal zählen
  SetVar(4,Sum(
    ObjectCount(SZ7K, 0,0,0,0, 0,"Overwatch",Var(0)),
    ObjectCount(SZ7K, 0,0,0,0, 0,"Overwatch2",Var())
  ));
//  DecVar(4);

  // Verschieben
  SetVertex(0,0,Sum(GetVertex(),Mul(Var(4),-2)));

  Special();

  return(1);


// ganz selten wird auch mal geblinzelt
Special:
  if(Not(Random(100)))
    SetAction("Overwatch2");

  return(1);