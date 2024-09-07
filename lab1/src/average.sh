numbers=($(cat "$1"))
count=${#numbers[@]}
sum=0
for num in "${numbers[@]}"; do
    sum=$((sum + num))
done
if [ $count -ne 0 ]; then
    average=$(echo "scale=2; $sum / $count" | bc)
else
    average=0
fi
echo "Количество чисел: $count"
echo "Среднее арифметическое: $average"