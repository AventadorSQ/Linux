echo "请输入用户名："
read user
if who | grep $user
then
		echo "用户已经登录系统!!!"
else
		echo "用户没有登录系统！！！"
fi
