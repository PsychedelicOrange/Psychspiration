Clear Host
param([string]$myFolder,[string]$StringA,[string]$StringB,[string]$prefixName)

Foreach ($file in Get-Childitem $myFolder)
{

$myFile = $file.name
$myFullFilename = $file.FullName
$myRenamed = $myFile.Replace("$StringA", "")
$myRenamed = $myRenamed.Replace("$StringB","")

Rename-Item $myFullFilename -newname $("{0}{1}" -f $prefixName, $myRenamed)

}