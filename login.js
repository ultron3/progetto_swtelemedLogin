console.log("script ok");

document.getElementById('submit').onclick = function (e) {
 
    var error = false;

    if(check('username')) error = true;
    if(check('password')) error = true;

    if(error) e.preventDefault();

}

function check(id) {

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

