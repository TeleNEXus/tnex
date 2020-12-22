
function validate(val)
{
  var sign = 1;
  

  DebugOut("JS Validator valmin = " + Attributes.valmin);
  DebugOut("JS Validator valmax = " + Attributes.valmax);

  if(typeof val !== "string") {return Invalid;}

  if(val.length === 0) return Intermediate;

  if(val.charAt(0) === "-"){
    sign = -1;
    // val = val.slice(1, val.length);
    val = val.slice(1);
    DebugOut("JS Validator val = " + val);
  }

  if(val.length === 0) return Intermediate;

  if(val === "0x") return Intermediate;

  var regExp = new RegExp("((^(0x){1,1})([a-fA-F0-9]*)$)|(^([0-9]*)$)");

  if(regExp.test(val) === false) {
    DebugOut("JS RegExpt fault.");
    return Invalid;
  }

  var data = parseInt(val);

  DebugOut("JS Validator parse to int = " + sign*data);
  if(isNaN(data)) {
    DebugOut("JS data is NaN!");
    return Invalid;
  }

  if((data > Attributes.valmax)||(data < Attributes.valmin)) return Invalid;
  return Acceptable;
}

DebugOut("Jave Script Evaluate");
