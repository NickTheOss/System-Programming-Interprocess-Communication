
disFile=$1
countriesFile=$2
input_dir=$3
numFperD=$4         #numFilesperDirectory
numRperF=$5		   	#numRecordsperFile

letters=(a b c d e f g h i g k l m n o p q r s t u v w x y z)

function create_date()
{
	day=$(((RANDOM % 30)+1))
	month=$(((RANDOM % 12)+1))
	year=$(((RANDOM % 31)+1990))
	test="${day}-${month}-${year}"
	echo $test	
}

function create_name()
{
	start=0
	end=$(((RANDOM % 9)+3))
	for (( c=0; c<=$end; c++ ))
	do
		char=${letters[$(((RANDOM % 26)+1))]}
		res="${res}${char}"	

	done
	echo $res
}

function ENTER_EXIT()
{

	flag=$((RANDOM % 2))
	if [ "$flag" -eq 1 ]; then

		output="ENTER"

	else

		output="EXIT"

	fi
	echo $output 

}

function generate_disease(){

	disease_num=$(((RANDOM % ${#diseases[@]})-1))
	echo ${diseases[disease_num]}

}


function generate_age()
{

	age=$(((RANDOM % 120)+1))
	echo $age

}



if [ -z "$disFile" ] || [ -z "$countriesFile" ] || [ -z "$input_dir" ] || [ -z "$numFperD" ] || [ -z "$numRperF" ] ; then # an den exei dothei kapoio orisma
	echo Wrong arguments.
	exit 1
fi

if [ ! -f "$disFile" ] || [ ! -f "$countriesFile" ]; then	# An den iparxoun ta arxeia
	echo Files do not exist
	exit 1
fi

re='^[-+]?[0-9]+$'
if ! [[ $numFperD =~ $re ]] || ! [[ $numRperF =~ $re ]]; then	# An oi arithmoi exoun dothei se lathos thesi
   echo error: Not a number.
   exit 1
fi

if [ "$numFperD" -le 0 ] || [ "$numRperF" -le 0 ]; then		# An oi arithmoi deneinai thetikoi
	echo Numbers must be greater than zero.
	exit 1
fi

if [ ! -d "$input_dir" ] ; then		# an den iparxei o fakelos dimiourgise ton
	mkdir $input_dir
else 								# Alliws sviston kai ksanaftiakston
	rm -rf $input_dir
	mkdir $input_dir
fi

# Gemizoume ta countries paths
count=0
while IFS= read -r line; do 	
	line="${input_dir}/${line}" # string concatenation
	countries[$count]=$line		
	mkdir ${countries[$count]}	# dimiourgia ipofakelwn
	count=$(($count + 1))		
done < $countriesFile

folder_counter=$((${#countries[@]}*$numFperD)) 
echo "Total subdirectories created = $folder_counter"

#dimiourgoume ta paths me ta arxeia kathe xwras
count=0
for (( c=0; c<$numFperD; c++ ))
do
	for (( d=0; d<${#countries[@]}; d++ ))
	do
		#path = $countries[$d]
		date=$(create_date)
		filesarray[$count]="${countries[$d]}/${date}"
		touch ${filesarray[$count]}
		count=$(($count + 1))
	done

done


# Kanoume to idio kai gia ton pinaka me tis astheneies
count=0
while IFS= read -r line; do
	diseases[$count]=$line
	count=$(($count + 1))
done < $disFile

# Eisagoume tis eggrafes me to format pou ipodiknietai stin ekfwnisi
count=0
for (( c=0; c<${#filesarray[@]}; c++ ))
do
	for (( d=0; d<$numRperF; d++ ))
	do
		echo "$count $(ENTER_EXIT) $(create_name) $(create_name) $(generate_disease) $(generate_age)" >> ${filesarray[$c]}
		count=$(($count + 1))
	done

done

