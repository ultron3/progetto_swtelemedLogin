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

