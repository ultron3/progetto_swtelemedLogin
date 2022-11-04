function Registrati()
{
    var username = document.modulo.username.value;
    if (username == "")
    {
        alert("Inserisci una username!");
    }
    else if (ControlloUsername(username) == false)
    {
        alert("Hai scelto una username riservata!");
    }
    else
    {
        alert("OK");
    }
}

function ControlloUsername(QualeUsername)
{
    var elenco = "admin administrator amministratore";
    var negate = elenco.split(" ");
    for (var u=0; u<negate.length; u++)
    {
        if (QualeUsername == negate[u])
        {
            return false;
            break;
        }
    }
}

console.log("script ok");

document.getElementById('submit').onclick = function (e) {
 
    var error = false;

    if(login('usr')) error = true;
    if(login('pwd')) error = true;

    if(error) e.preventDefault();

}

function login(id) {

if(document.getElementById(id).value == '')
{
document.getElementById('error_' + id).innerHTML = 'Campo obbligatorio';
return true;
}
else
{
document.getElementById('error_' + id).innerHTML = '';
return false;
}

}

