fileBase=180221
startPulse=180221017
startFile=1
numImages=5

for i in `seq 1 $numImages`; 
do

    newFile=$(printf "/home/fuze/DHI_Images/%s/%d.JPG" "$fileBase" "$startPulse")
    oldFile=$(printf "/home/fuze/DHI_Images/%s/DSC_00%02d.JPG" "$fileBase" "$startFile")
    mv -i -- "$oldFile" "$newFile"
    echo "Moving $oldFile to $newFile"
    let startPulse=startPulse+1
    let startFile=startFile+1

done
