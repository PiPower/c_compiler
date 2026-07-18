

define dso_local i8 @test_signed_char() #0 {
	%2 = alloca i8, align 1
	%3 = alloca i8, align 1
	store i8 100, ptr %3, align 1
	%4 = alloca i8, align 1
	store i8 7, ptr %4, align 1
	%5 = load i8, ptr %3, align 1
	%6 = load i8, ptr %4, align 1
	%7 = sext i8 %5 to i32
	%8 = sext i8 %6 to i32
	%9 = add nsw i32 %7, %8
	%10 = add nsw i32 %9, 2
	%11 = add nsw i32 %10, 4
	%12 = trunc i32 %11 to i8
	store i8 %12, ptr %3, align 1
	%13 = load i8, ptr %3, align 1
	%14 = load i8, ptr %4, align 1
	%15 = sext i8 %13 to i32
	%16 = sext i8 %14 to i32
	%17 = sub nsw i32 %15, %16
	%18 = sub nsw i32 %17, 22
	%19 = trunc i32 %18 to i8
	store i8 %19, ptr %3, align 1
	%20 = load i8, ptr %3, align 1
	%21 = load i8, ptr %4, align 1
	%22 = sext i8 %20 to i32
	%23 = sext i8 %21 to i32
	%24 = mul nsw i32 %22, %23
	%25 = trunc i32 %24 to i8
	store i8 %25, ptr %3, align 1
	%26 = load i8, ptr %3, align 1
	%27 = load i8, ptr %4, align 1
	%28 = sext i8 %26 to i32
	%29 = sext i8 %27 to i32
	%30 = sdiv i32 %28, %29
	%31 = trunc i32 %30 to i8
	store i8 %31, ptr %3, align 1
	%32 = load i8, ptr %3, align 1
	%33 = load i8, ptr %4, align 1
	%34 = sext i8 %32 to i32
	%35 = sext i8 %33 to i32
	%36 = srem i32 %34, %35
	%37 = trunc i32 %36 to i8
	store i8 %37, ptr %3, align 1
	%38 = load i8, ptr %3, align 1
	%39 = load i8, ptr %4, align 1
	%40 = sext i8 %38 to i32
	%41 = sext i8 %39 to i32
	%42 = and i32 %40, %41
	%43 = trunc i32 %42 to i8
	store i8 %43, ptr %3, align 1
	%44 = load i8, ptr %3, align 1
	%45 = load i8, ptr %4, align 1
	%46 = sext i8 %44 to i32
	%47 = sext i8 %45 to i32
	%48 = or i32 %46, %47
	%49 = trunc i32 %48 to i8
	store i8 %49, ptr %3, align 1
	%50 = load i8, ptr %3, align 1
	%51 = load i8, ptr %4, align 1
	%52 = sext i8 %50 to i32
	%53 = sext i8 %51 to i32
	%54 = xor i32 %52, %53
	%55 = trunc i32 %54 to i8
	store i8 %55, ptr %3, align 1
	%56 = load i8, ptr %3, align 1
	%57 = sext i8 %56 to i32
	%58 = shl i32 %57, 2
	%59 = trunc i32 %58 to i8
	store i8 %59, ptr %3, align 1
	%60 = load i8, ptr %3, align 1
	%61 = sext i8 %60 to i32
	%62 = ashr i32 %61, 1
	%63 = trunc i32 %62 to i8
	store i8 %63, ptr %3, align 1
	store i8 %-20000, ptr %2, align 1
	br label %label_1

label_1:
	%64 = load i8, ptr %2, align 1
	ret i8 %64
}

define dso_local i8 @test_unsigned_char() #0 {
	%2 = alloca i8, align 1
	%3 = alloca i8, align 1
	store i8 -56, ptr %3, align 1
	%4 = alloca i8, align 1
	store i8 13, ptr %4, align 1
	%5 = load i8, ptr %3, align 1
	%6 = load i8, ptr %4, align 1
	%7 = zext i8 %5 to i32
	%8 = zext i8 %6 to i32
	%9 = add nsw i32 %7, %8
	%10 = trunc i32 %9 to i8
	store i8 %10, ptr %3, align 1
	%11 = load i8, ptr %3, align 1
	%12 = load i8, ptr %4, align 1
	%13 = zext i8 %11 to i32
	%14 = zext i8 %12 to i32
	%15 = sub nsw i32 %13, %14
	%16 = trunc i32 %15 to i8
	store i8 %16, ptr %3, align 1
	%17 = load i8, ptr %3, align 1
	%18 = load i8, ptr %4, align 1
	%19 = zext i8 %17 to i32
	%20 = zext i8 %18 to i32
	%21 = mul nsw i32 %19, %20
	%22 = trunc i32 %21 to i8
	store i8 %22, ptr %3, align 1
	%23 = load i8, ptr %3, align 1
	%24 = load i8, ptr %4, align 1
	%25 = zext i8 %23 to i32
	%26 = zext i8 %24 to i32
	%27 = sdiv i32 %25, %26
	%28 = trunc i32 %27 to i8
	store i8 %28, ptr %3, align 1
	%29 = load i8, ptr %3, align 1
	%30 = load i8, ptr %4, align 1
	%31 = zext i8 %29 to i32
	%32 = zext i8 %30 to i32
	%33 = srem i32 %31, %32
	%34 = trunc i32 %33 to i8
	store i8 %34, ptr %3, align 1
	%35 = load i8, ptr %3, align 1
	%36 = load i8, ptr %4, align 1
	%37 = zext i8 %35 to i32
	%38 = zext i8 %36 to i32
	%39 = and i32 %37, %38
	%40 = trunc i32 %39 to i8
	store i8 %40, ptr %3, align 1
	%41 = load i8, ptr %3, align 1
	%42 = load i8, ptr %4, align 1
	%43 = zext i8 %41 to i32
	%44 = zext i8 %42 to i32
	%45 = or i32 %43, %44
	%46 = trunc i32 %45 to i8
	store i8 %46, ptr %3, align 1
	%47 = load i8, ptr %3, align 1
	%48 = load i8, ptr %4, align 1
	%49 = zext i8 %47 to i32
	%50 = zext i8 %48 to i32
	%51 = xor i32 %49, %50
	%52 = trunc i32 %51 to i8
	store i8 %52, ptr %3, align 1
	%53 = load i8, ptr %3, align 1
	%54 = zext i8 %53 to i32
	%55 = shl i32 %54, 2
	%56 = trunc i32 %55 to i8
	store i8 %56, ptr %3, align 1
	%57 = load i8, ptr %3, align 1
	%58 = zext i8 %57 to i32
	%59 = ashr i32 %58, 1
	%60 = trunc i32 %59 to i8
	store i8 %60, ptr %3, align 1
	store i8 %-20000, ptr %2, align 1
	br label %label_1

label_1:
	%61 = load i8, ptr %2, align 1
	ret i8 %61
}

define dso_local i16 @test_short() #0 {
	%2 = alloca i16, align 2
	%3 = alloca i16, align 2
	store i16 1234, ptr %3, align 2
	%4 = alloca i16, align 2
	store i16 37, ptr %4, align 2
	%5 = load i16, ptr %3, align 2
	%6 = load i16, ptr %4, align 2
	%7 = sext i16 %5 to i32
	%8 = sext i16 %6 to i32
	%9 = add nsw i32 %7, %8
	%10 = trunc i32 %9 to i16
	store i16 %10, ptr %3, align 2
	%11 = load i16, ptr %3, align 2
	%12 = load i16, ptr %4, align 2
	%13 = sext i16 %11 to i32
	%14 = sext i16 %12 to i32
	%15 = sub nsw i32 %13, %14
	%16 = trunc i32 %15 to i16
	store i16 %16, ptr %3, align 2
	%17 = load i16, ptr %3, align 2
	%18 = load i16, ptr %4, align 2
	%19 = sext i16 %17 to i32
	%20 = sext i16 %18 to i32
	%21 = mul nsw i32 %19, %20
	%22 = trunc i32 %21 to i16
	store i16 %22, ptr %3, align 2
	%23 = load i16, ptr %3, align 2
	%24 = load i16, ptr %4, align 2
	%25 = sext i16 %23 to i32
	%26 = sext i16 %24 to i32
	%27 = sdiv i32 %25, %26
	%28 = trunc i32 %27 to i16
	store i16 %28, ptr %3, align 2
	%29 = load i16, ptr %3, align 2
	%30 = load i16, ptr %4, align 2
	%31 = sext i16 %29 to i32
	%32 = sext i16 %30 to i32
	%33 = srem i32 %31, %32
	%34 = trunc i32 %33 to i16
	store i16 %34, ptr %3, align 2
	%35 = load i16, ptr %3, align 2
	%36 = load i16, ptr %4, align 2
	%37 = sext i16 %35 to i32
	%38 = sext i16 %36 to i32
	%39 = and i32 %37, %38
	%40 = trunc i32 %39 to i16
	store i16 %40, ptr %3, align 2
	%41 = load i16, ptr %3, align 2
	%42 = load i16, ptr %4, align 2
	%43 = sext i16 %41 to i32
	%44 = sext i16 %42 to i32
	%45 = or i32 %43, %44
	%46 = trunc i32 %45 to i16
	store i16 %46, ptr %3, align 2
	%47 = load i16, ptr %3, align 2
	%48 = load i16, ptr %4, align 2
	%49 = sext i16 %47 to i32
	%50 = sext i16 %48 to i32
	%51 = xor i32 %49, %50
	%52 = trunc i32 %51 to i16
	store i16 %52, ptr %3, align 2
	%53 = load i16, ptr %3, align 2
	%54 = sext i16 %53 to i32
	%55 = shl i32 %54, 2
	%56 = trunc i32 %55 to i16
	store i16 %56, ptr %3, align 2
	%57 = load i16, ptr %3, align 2
	%58 = sext i16 %57 to i32
	%59 = ashr i32 %58, 1
	%60 = trunc i32 %59 to i16
	store i16 %60, ptr %3, align 2
	store i16 %-20000, ptr %2, align 2
	br label %label_1

label_1:
	%61 = load i16, ptr %2, align 2
	ret i16 %61
}

define dso_local i16 @test_unsigned_short() #0 {
	%2 = alloca i16, align 2
	%3 = alloca i16, align 2
	store i16 -11215, ptr %3, align 2
	%4 = alloca i16, align 2
	store i16 123, ptr %4, align 2
	%5 = load i16, ptr %3, align 2
	%6 = load i16, ptr %4, align 2
	%7 = add nsw i32 %5, %6
	store i16 %7, ptr %3, align 2
	%8 = load i16, ptr %3, align 2
	%9 = load i16, ptr %4, align 2
	%10 = sub nsw i32 %8, %9
	store i16 %10, ptr %3, align 2
	%11 = load i16, ptr %3, align 2
	%12 = load i16, ptr %4, align 2
	%13 = mul nsw i32 %11, %12
	store i16 %13, ptr %3, align 2
	%14 = load i16, ptr %3, align 2
	%15 = load i16, ptr %4, align 2
	%16 = sdiv i32 %14, %15
	store i16 %16, ptr %3, align 2
	%17 = load i16, ptr %3, align 2
	%18 = load i16, ptr %4, align 2
	%19 = srem i32 %17, %18
	store i16 %19, ptr %3, align 2
	%20 = load i16, ptr %3, align 2
	%21 = load i16, ptr %4, align 2
	%22 = and i32 %20, %21
	store i16 %22, ptr %3, align 2
	%23 = load i16, ptr %3, align 2
	%24 = load i16, ptr %4, align 2
	%25 = or i32 %23, %24
	store i16 %25, ptr %3, align 2
	%26 = load i16, ptr %3, align 2
	%27 = load i16, ptr %4, align 2
	%28 = xor i32 %26, %27
	store i16 %28, ptr %3, align 2
	%29 = load i16, ptr %3, align 2
	%30 = shl i16 %29, 2
	store i16 %30, ptr %3, align 2
	%31 = load i16, ptr %3, align 2
	%32 = lshr i16 %31, 1
	store i16 %32, ptr %3, align 2
	store i16 %-20000, ptr %2, align 2
	br label %label_1

label_1:
	%33 = load i16, ptr %2, align 2
	ret i16 %33
}

define dso_local i32 @test_int() #0 {
	%2 = alloca i32, align 4
	%3 = alloca i32, align 4
	store i32 123456, ptr %3, align 4
	%4 = alloca i32, align 4
	store i32 789, ptr %4, align 4
	%5 = load i32, ptr %3, align 4
	%6 = load i32, ptr %4, align 4
	%7 = add nsw i32 %5, %6
	store i32 %7, ptr %3, align 4
	%8 = load i32, ptr %3, align 4
	%9 = load i32, ptr %4, align 4
	%10 = sub nsw i32 %8, %9
	store i32 %10, ptr %3, align 4
	%11 = load i32, ptr %3, align 4
	%12 = load i32, ptr %4, align 4
	%13 = mul nsw i32 %11, %12
	store i32 %13, ptr %3, align 4
	%14 = load i32, ptr %3, align 4
	%15 = load i32, ptr %4, align 4
	%16 = sdiv i32 %14, %15
	store i32 %16, ptr %3, align 4
	%17 = load i32, ptr %3, align 4
	%18 = load i32, ptr %4, align 4
	%19 = srem i32 %17, %18
	store i32 %19, ptr %3, align 4
	%20 = load i32, ptr %3, align 4
	%21 = load i32, ptr %4, align 4
	%22 = and i32 %20, %21
	store i32 %22, ptr %3, align 4
	%23 = load i32, ptr %3, align 4
	%24 = load i32, ptr %4, align 4
	%25 = or i32 %23, %24
	store i32 %25, ptr %3, align 4
	%26 = load i32, ptr %3, align 4
	%27 = load i32, ptr %4, align 4
	%28 = xor i32 %26, %27
	store i32 %28, ptr %3, align 4
	%29 = load i32, ptr %3, align 4
	%30 = shl i32 %29, 3
	store i32 %30, ptr %3, align 4
	%31 = load i32, ptr %3, align 4
	%32 = ashr i32 %31, 2
	store i32 %32, ptr %3, align 4
	store i32 %-20000, ptr %2, align 4
	br label %label_1

label_1:
	%33 = load i32, ptr %2, align 4
	ret i32 %33
}

define dso_local i32 @test_unsigned_int() #0 {
	%2 = alloca i32, align 4
	%3 = alloca i32, align 4
	store i32 -294967296, ptr %3, align 4
	%4 = alloca i32, align 4
	store i32 12345, ptr %4, align 4
	%5 = load i32, ptr %3, align 4
	%6 = load i32, ptr %4, align 4
	%7 = add i32 %5, %6
	store i32 %7, ptr %3, align 4
	%8 = load i32, ptr %3, align 4
	%9 = load i32, ptr %4, align 4
	%10 = sub i32 %8, %9
	store i32 %10, ptr %3, align 4
	%11 = load i32, ptr %3, align 4
	%12 = load i32, ptr %4, align 4
	%13 = mul i32 %11, %12
	store i32 %13, ptr %3, align 4
	%14 = load i32, ptr %3, align 4
	%15 = load i32, ptr %4, align 4
	%16 = udiv i32 %14, %15
	store i32 %16, ptr %3, align 4
	%17 = load i32, ptr %3, align 4
	%18 = load i32, ptr %4, align 4
	%19 = urem i32 %17, %18
	store i32 %19, ptr %3, align 4
	%20 = load i32, ptr %3, align 4
	%21 = load i32, ptr %4, align 4
	%22 = and i32 %20, %21
	store i32 %22, ptr %3, align 4
	%23 = load i32, ptr %3, align 4
	%24 = load i32, ptr %4, align 4
	%25 = or i32 %23, %24
	store i32 %25, ptr %3, align 4
	%26 = load i32, ptr %3, align 4
	%27 = load i32, ptr %4, align 4
	%28 = xor i32 %26, %27
	store i32 %28, ptr %3, align 4
	%29 = load i32, ptr %3, align 4
	%30 = shl i32 %29, 3
	store i32 %30, ptr %3, align 4
	%31 = load i32, ptr %3, align 4
	%32 = lshr i32 %31, 2
	store i32 %32, ptr %3, align 4
	store i32 %-20000, ptr %2, align 4
	br label %label_1

label_1:
	%33 = load i32, ptr %2, align 4
	ret i32 %33
}

define dso_local i64 @test_long() #0 {
	%2 = alloca i64, align 8
	%3 = alloca i64, align 8
	store i64 123456789, ptr %3, align 8
	%4 = alloca i64, align 8
	store i64 1234, ptr %4, align 8
	%5 = load i64, ptr %3, align 8
	%6 = load i64, ptr %4, align 8
	%7 = add nsw i64 %5, %6
	store i64 %7, ptr %3, align 8
	%8 = load i64, ptr %3, align 8
	%9 = load i64, ptr %4, align 8
	%10 = sub nsw i64 %8, %9
	store i64 %10, ptr %3, align 8
	%11 = load i64, ptr %3, align 8
	%12 = load i64, ptr %4, align 8
	%13 = mul nsw i64 %11, %12
	store i64 %13, ptr %3, align 8
	%14 = load i64, ptr %3, align 8
	%15 = load i64, ptr %4, align 8
	%16 = sdiv i64 %14, %15
	store i64 %16, ptr %3, align 8
	%17 = load i64, ptr %3, align 8
	%18 = load i64, ptr %4, align 8
	%19 = srem i64 %17, %18
	store i64 %19, ptr %3, align 8
	%20 = load i64, ptr %3, align 8
	%21 = load i64, ptr %4, align 8
	%22 = and i64 %20, %21
	store i64 %22, ptr %3, align 8
	%23 = load i64, ptr %3, align 8
	%24 = load i64, ptr %4, align 8
	%25 = or i64 %23, %24
	store i64 %25, ptr %3, align 8
	%26 = load i64, ptr %3, align 8
	%27 = load i64, ptr %4, align 8
	%28 = xor i64 %26, %27
	store i64 %28, ptr %3, align 8
	%29 = load i64, ptr %3, align 8
	%30 = shl i64 %29, 4
	store i64 %30, ptr %3, align 8
	%31 = load i64, ptr %3, align 8
	%32 = ashr i64 %31, 3
	store i64 %32, ptr %3, align 8
	store i64 %-20000, ptr %2, align 8
	br label %label_1

label_1:
	%33 = load i64, ptr %2, align 8
	ret i64 %33
}

define dso_local i64 @test_unsigned_long() #0 {
	%2 = alloca i64, align 8
	%3 = alloca i64, align 8
	store i64 3000000000, ptr %3, align 8
	%4 = alloca i64, align 8
	store i64 777, ptr %4, align 8
	%5 = load i64, ptr %3, align 8
	%6 = load i64, ptr %4, align 8
	%7 = add i64 %5, %6
	store i64 %7, ptr %3, align 8
	%8 = load i64, ptr %3, align 8
	%9 = load i64, ptr %4, align 8
	%10 = sub i64 %8, %9
	store i64 %10, ptr %3, align 8
	%11 = load i64, ptr %3, align 8
	%12 = load i64, ptr %4, align 8
	%13 = mul i64 %11, %12
	store i64 %13, ptr %3, align 8
	%14 = load i64, ptr %3, align 8
	%15 = load i64, ptr %4, align 8
	%16 = udiv i64 %14, %15
	store i64 %16, ptr %3, align 8
	%17 = load i64, ptr %3, align 8
	%18 = load i64, ptr %4, align 8
	%19 = urem i64 %17, %18
	store i64 %19, ptr %3, align 8
	%20 = load i64, ptr %3, align 8
	%21 = load i64, ptr %4, align 8
	%22 = and i64 %20, %21
	store i64 %22, ptr %3, align 8
	%23 = load i64, ptr %3, align 8
	%24 = load i64, ptr %4, align 8
	%25 = or i64 %23, %24
	store i64 %25, ptr %3, align 8
	%26 = load i64, ptr %3, align 8
	%27 = load i64, ptr %4, align 8
	%28 = xor i64 %26, %27
	store i64 %28, ptr %3, align 8
	%29 = load i64, ptr %3, align 8
	%30 = shl i64 %29, 4
	store i64 %30, ptr %3, align 8
	%31 = load i64, ptr %3, align 8
	%32 = lshr i64 %31, 3
	store i64 %32, ptr %3, align 8
	store i64 %-20000, ptr %2, align 8
	br label %label_1

label_1:
	%33 = load i64, ptr %2, align 8
	ret i64 %33
}

define dso_local i64 @test_long_long() #0 {
	%2 = alloca i64, align 8
	%3 = alloca i64, align 8
	store i64 1234567890123, ptr %3, align 8
	%4 = alloca i64, align 8
	store i64 4567, ptr %4, align 8
	%5 = load i64, ptr %3, align 8
	%6 = load i64, ptr %4, align 8
	%7 = add nsw i64 %5, %6
	store i64 %7, ptr %3, align 8
	%8 = load i64, ptr %3, align 8
	%9 = load i64, ptr %4, align 8
	%10 = sub nsw i64 %8, %9
	store i64 %10, ptr %3, align 8
	%11 = load i64, ptr %3, align 8
	%12 = load i64, ptr %4, align 8
	%13 = mul nsw i64 %11, %12
	store i64 %13, ptr %3, align 8
	%14 = load i64, ptr %3, align 8
	%15 = load i64, ptr %4, align 8
	%16 = sdiv i64 %14, %15
	store i64 %16, ptr %3, align 8
	%17 = load i64, ptr %3, align 8
	%18 = load i64, ptr %4, align 8
	%19 = srem i64 %17, %18
	store i64 %19, ptr %3, align 8
	%20 = load i64, ptr %3, align 8
	%21 = load i64, ptr %4, align 8
	%22 = and i64 %20, %21
	store i64 %22, ptr %3, align 8
	%23 = load i64, ptr %3, align 8
	%24 = load i64, ptr %4, align 8
	%25 = or i64 %23, %24
	store i64 %25, ptr %3, align 8
	%26 = load i64, ptr %3, align 8
	%27 = load i64, ptr %4, align 8
	%28 = xor i64 %26, %27
	store i64 %28, ptr %3, align 8
	%29 = load i64, ptr %3, align 8
	%30 = shl i64 %29, 5
	store i64 %30, ptr %3, align 8
	%31 = load i64, ptr %3, align 8
	%32 = ashr i64 %31, 4
	store i64 %32, ptr %3, align 8
	store i64 %-20000, ptr %2, align 8
	br label %label_1

label_1:
	%33 = load i64, ptr %2, align 8
	ret i64 %33
}

define dso_local i64 @test_unsigned_long_long() #0 {
	%2 = alloca i64, align 8
	%3 = alloca i64, align 8
	store i64 123456789012345, ptr %3, align 8
	%4 = alloca i64, align 8
	store i64 98765, ptr %4, align 8
	%5 = load i64, ptr %3, align 8
	%6 = load i64, ptr %4, align 8
	%7 = add i64 %5, %6
	store i64 %7, ptr %3, align 8
	%8 = load i64, ptr %3, align 8
	%9 = load i64, ptr %4, align 8
	%10 = sub i64 %8, %9
	store i64 %10, ptr %3, align 8
	%11 = load i64, ptr %3, align 8
	%12 = load i64, ptr %4, align 8
	%13 = mul i64 %11, %12
	store i64 %13, ptr %3, align 8
	%14 = load i64, ptr %3, align 8
	%15 = load i64, ptr %4, align 8
	%16 = udiv i64 %14, %15
	store i64 %16, ptr %3, align 8
	%17 = load i64, ptr %3, align 8
	%18 = load i64, ptr %4, align 8
	%19 = urem i64 %17, %18
	store i64 %19, ptr %3, align 8
	%20 = load i64, ptr %3, align 8
	%21 = load i64, ptr %4, align 8
	%22 = and i64 %20, %21
	store i64 %22, ptr %3, align 8
	%23 = load i64, ptr %3, align 8
	%24 = load i64, ptr %4, align 8
	%25 = or i64 %23, %24
	store i64 %25, ptr %3, align 8
	%26 = load i64, ptr %3, align 8
	%27 = load i64, ptr %4, align 8
	%28 = xor i64 %26, %27
	store i64 %28, ptr %3, align 8
	%29 = load i64, ptr %3, align 8
	%30 = shl i64 %29, 5
	store i64 %30, ptr %3, align 8
	%31 = load i64, ptr %3, align 8
	%32 = lshr i64 %31, 4
	store i64 %32, ptr %3, align 8
	store i64 %-20000, ptr %2, align 8
	br label %label_1

label_1:
	%33 = load i64, ptr %2, align 8
	ret i64 %33
}

define dso_local float @test_float() #0 {
	%2 = alloca float, align 4
	%3 = alloca float, align 4
	store float 3.500000e+00, ptr %3, align 4
	%4 = alloca float, align 4
	store float 1.250000e+00, ptr %4, align 4
	%5 = load float, ptr %3, align 4
	%6 = load float, ptr %4, align 4
	%7 = fadd float %5, %6
	store float %7, ptr %3, align 4
	%8 = load float, ptr %3, align 4
	%9 = load float, ptr %4, align 4
	%10 = fsub float %8, %9
	store float %10, ptr %3, align 4
	%11 = load float, ptr %3, align 4
	%12 = load float, ptr %4, align 4
	%13 = fmul float %11, %12
	store float %13, ptr %3, align 4
	%14 = load float, ptr %3, align 4
	%15 = load float, ptr %4, align 4
	%16 = fdiv float %14, %15
	store float %16, ptr %3, align 4
	store float %-20000, ptr %2, align 4
	br label %label_1

label_1:
	%17 = load float, ptr %2, align 4
	ret float %17
}

define dso_local double @test_double() #0 {
	%2 = alloca double, align 8
	%3 = alloca double, align 8
	store double 3.141593e+00, ptr %3, align 8
	%4 = alloca double, align 8
	store double 2.718282e+00, ptr %4, align 8
	%5 = load double, ptr %3, align 8
	%6 = load double, ptr %4, align 8
	%7 = fadd double %5, %6
	store double %7, ptr %3, align 8
	%8 = load double, ptr %3, align 8
	%9 = load double, ptr %4, align 8
	%10 = fsub double %8, %9
	store double %10, ptr %3, align 8
	%11 = load double, ptr %3, align 8
	%12 = load double, ptr %4, align 8
	%13 = fmul double %11, %12
	store double %13, ptr %3, align 8
	%14 = load double, ptr %3, align 8
	%15 = load double, ptr %4, align 8
	%16 = fdiv double %14, %15
	store double %16, ptr %3, align 8
	store double %-20000, ptr %2, align 8
	br label %label_1

label_1:
	%17 = load double, ptr %2, align 8
	ret double %17
}

define dso_local x86_fp80 @test_long_double() #0 {
	%2 = alloca x86_fp80, align 16
	%3 = alloca x86_fp80, align 16
	store x86_fp80 f0x3FFF9E06521462CFDC1A, ptr %3, align 16
	%4 = alloca x86_fp80, align 16
	store x86_fp80 f0x40029E06522C8B909A4C, ptr %4, align 16
	%5 = load x86_fp80, ptr %3, align 16
	%6 = load x86_fp80, ptr %4, align 16
	%7 = fadd x86_fp80 %5, %6
	store x86_fp80 %7, ptr %3, align 16
	%8 = load x86_fp80, ptr %3, align 16
	%9 = load x86_fp80, ptr %4, align 16
	%10 = fsub x86_fp80 %8, %9
	store x86_fp80 %10, ptr %3, align 16
	%11 = load x86_fp80, ptr %3, align 16
	%12 = load x86_fp80, ptr %4, align 16
	%13 = fmul x86_fp80 %11, %12
	store x86_fp80 %13, ptr %3, align 16
	%14 = load x86_fp80, ptr %3, align 16
	%15 = load x86_fp80, ptr %4, align 16
	%16 = fdiv x86_fp80 %14, %15
	store x86_fp80 %16, ptr %3, align 16
	store x86_fp80 %-20000, ptr %2, align 16
	br label %label_1

label_1:
	%17 = load x86_fp80, ptr %2, align 16
	ret x86_fp80 %17
}

define dso_local i32 @test_int_mixed() #0 {
	%2 = alloca i32, align 4
	%3 = alloca i32, align 4
	store i32 1000, ptr %3, align 4
	%4 = alloca i32, align 4
	store i32 4000, ptr %4, align 4
	%5 = load i32, ptr %3, align 4
	%6 = load i32, ptr %4, align 4
	%7 = sext i32 %5 to i32
	%8 = add i32 %7, %6
	%9 = trunc i32 %8 to i32
	store i32 %9, ptr %3, align 4
	%10 = load i32, ptr %3, align 4
	%11 = load i32, ptr %4, align 4
	%12 = sext i32 %10 to i32
	%13 = sub i32 %12, %11
	%14 = trunc i32 %13 to i32
	store i32 %14, ptr %3, align 4
	%15 = load i32, ptr %3, align 4
	%16 = load i32, ptr %4, align 4
	%17 = sext i32 %15 to i32
	%18 = mul i32 %17, %16
	%19 = trunc i32 %18 to i32
	store i32 %19, ptr %3, align 4
	%20 = load i32, ptr %3, align 4
	%21 = load i32, ptr %4, align 4
	%22 = sext i32 %20 to i32
	%23 = udiv i32 %22, %21
	%24 = trunc i32 %23 to i32
	store i32 %24, ptr %3, align 4
	%25 = load i32, ptr %3, align 4
	%26 = load i32, ptr %4, align 4
	%27 = sext i32 %25 to i32
	%28 = add i32 %27, %26
	%29 = trunc i32 %28 to i32
	store i32 %29, ptr %3, align 4
	%30 = load i32, ptr %3, align 4
	%31 = load i32, ptr %4, align 4
	%32 = sext i32 %30 to i32
	%33 = sub i32 %32, %31
	%34 = trunc i32 %33 to i32
	store i32 %34, ptr %3, align 4
	store i32 %-20000, ptr %2, align 4
	br label %label_1

label_1:
	%35 = load i32, ptr %2, align 4
	ret i32 %35
}

define dso_local i32 @test_unsigned_int_mixed() #0 {
	%2 = alloca i32, align 4
	%3 = alloca i32, align 4
	store i32 3000, ptr %3, align 4
	%4 = alloca i32, align 4
	store i32 -1200, ptr %4, align 4
	%5 = load i32, ptr %3, align 4
	%6 = load i32, ptr %4, align 4
	%7 = sext i32 %6 to i32
	%8 = add i32 %5, %7
	store i32 %8, ptr %3, align 4
	%9 = load i32, ptr %3, align 4
	%10 = load i32, ptr %4, align 4
	%11 = sext i32 %10 to i32
	%12 = sub i32 %9, %11
	store i32 %12, ptr %3, align 4
	%13 = load i32, ptr %3, align 4
	%14 = load i32, ptr %4, align 4
	%15 = sext i32 %14 to i32
	%16 = mul i32 %13, %15
	store i32 %16, ptr %3, align 4
	%17 = load i32, ptr %3, align 4
	%18 = load i32, ptr %4, align 4
	%19 = sext i32 %18 to i32
	%20 = udiv i32 %17, %19
	store i32 %20, ptr %3, align 4
	%21 = load i32, ptr %3, align 4
	%22 = load i32, ptr %4, align 4
	%23 = sext i32 %22 to i32
	%24 = add i32 %21, %23
	store i32 %24, ptr %3, align 4
	%25 = trunc i32 %-20000 to i32
	store i32 %-20000, ptr %2, align 4
	br label %label_1

label_1:
	%26 = load i32, ptr %2, align 4
	ret i32 %26
}

define dso_local i32 @test_char_short_mixed() #0 {
	%2 = alloca i32, align 4
	%3 = alloca i8, align 1
	store i8 -10, ptr %3, align 1
	%4 = alloca i8, align 1
	store i8 -56, ptr %4, align 1
	%5 = alloca i16, align 2
	store i16 3000, ptr %5, align 2
	%6 = alloca i16, align 2
	store i16 -5536, ptr %6, align 2
	%7 = alloca i32, align 4
	%8 = load i8, ptr %3, align 1
	%9 = sext i8 %8 to i32
	store i32 %9, ptr %7, align 4
	%10 = load i32, ptr %7, align 4
	%11 = load i8, ptr %4, align 1
	%12 = zext i8 %11 to i32
	%13 = add nsw i32 %10, %12
	store i32 %13, ptr %7, align 4
	%14 = load i32, ptr %7, align 4
	%15 = load i16, ptr %5, align 2
	%16 = sext i16 %15 to i32
	%17 = add nsw i32 %14, %16
	store i32 %17, ptr %7, align 4
	%18 = load i32, ptr %7, align 4
	%19 = load i16, ptr %6, align 2
	%20 = add i16 %18, %19
	store i32 %20, ptr %7, align 4
	%21 = load i32, ptr %7, align 4
	%22 = load i8, ptr %3, align 1
	%23 = load i8, ptr %4, align 1
	%24 = sext i8 %22 to i32
	%25 = zext i8 %23 to i32
	%26 = add nsw i32 %24, %25
	%27 = add nsw i32 %21, %26
	store i32 %27, ptr %7, align 4
	%28 = load i32, ptr %7, align 4
	%29 = load i16, ptr %5, align 2
	%30 = load i16, ptr %6, align 2
	%31 = sext i16 %29 to i32
	%32 = add nsw i32 %31, %30
	%33 = add nsw i32 %28, %32
	store i32 %33, ptr %7, align 4
	store i32 %-20000, ptr %2, align 4
	br label %label_1

label_1:
	%34 = load i32, ptr %2, align 4
	ret i32 %34
}

define dso_local i64 @test_long_int_mixed() #0 {
	%2 = alloca i64, align 8
	%3 = alloca i64, align 8
	store i64 100000, ptr %3, align 8
	%4 = alloca i32, align 4
	store i32 -500, ptr %4, align 4
	%5 = load i64, ptr %3, align 8
	%6 = load i32, ptr %4, align 4
	%7 = sext i32 %6 to i64
	%8 = add nsw i64 %5, %7
	store i64 %8, ptr %3, align 8
	%9 = load i64, ptr %3, align 8
	%10 = load i32, ptr %4, align 4
	%11 = sext i32 %10 to i64
	%12 = sub nsw i64 %9, %11
	store i64 %12, ptr %3, align 8
	%13 = load i64, ptr %3, align 8
	%14 = load i32, ptr %4, align 4
	%15 = sext i32 %14 to i64
	%16 = mul nsw i64 %13, %15
	store i64 %16, ptr %3, align 8
	%17 = load i64, ptr %3, align 8
	%18 = load i32, ptr %4, align 4
	%19 = sext i32 %18 to i64
	%20 = sdiv i64 %17, %19
	store i64 %20, ptr %3, align 8
	%21 = load i64, ptr %3, align 8
	%22 = load i32, ptr %4, align 4
	%23 = sext i32 %22 to i64
	%24 = add nsw i64 %21, %23
	store i64 %24, ptr %3, align 8
	store i64 %-20000, ptr %2, align 8
	br label %label_1

label_1:
	%25 = load i64, ptr %2, align 8
	ret i64 %25
}

define dso_local i64 @test_ulong_int_mixed() #0 {
	%2 = alloca i64, align 8
	%3 = alloca i64, align 8
	store i64 3000000000, ptr %3, align 8
	%4 = alloca i32, align 4
	store i32 -1000, ptr %4, align 4
	%5 = load i64, ptr %3, align 8
	%6 = load i32, ptr %4, align 4
	%7 = sext i32 %6 to i64
	%8 = add i64 %5, %7
	store i64 %8, ptr %3, align 8
	%9 = load i64, ptr %3, align 8
	%10 = load i32, ptr %4, align 4
	%11 = sext i32 %10 to i64
	%12 = sub i64 %9, %11
	store i64 %12, ptr %3, align 8
	%13 = load i64, ptr %3, align 8
	%14 = load i32, ptr %4, align 4
	%15 = sext i32 %14 to i64
	%16 = mul i64 %13, %15
	store i64 %16, ptr %3, align 8
	%17 = load i64, ptr %3, align 8
	%18 = load i32, ptr %4, align 4
	%19 = sext i32 %18 to i64
	%20 = udiv i64 %17, %19
	store i64 %20, ptr %3, align 8
	%21 = load i64, ptr %3, align 8
	%22 = load i32, ptr %4, align 4
	%23 = sext i32 %22 to i64
	%24 = add i64 %21, %23
	store i64 %24, ptr %3, align 8
	store i64 %-20000, ptr %2, align 8
	br label %label_1

label_1:
	%25 = load i64, ptr %2, align 8
	ret i64 %25
}

define dso_local i64 @test_ll_mixed() #0 {
	%2 = alloca i64, align 8
	%3 = alloca i64, align 8
	store i64 5000000000, ptr %3, align 8
	%4 = alloca i64, align 8
	store i64 9000000000, ptr %4, align 8
	%5 = load i64, ptr %3, align 8
	%6 = load i64, ptr %4, align 8
	%7 = sext i64 %5 to i64
	%8 = add i64 %7, %6
	%9 = trunc i64 %8 to i64
	store i64 %9, ptr %3, align 8
	%10 = load i64, ptr %3, align 8
	%11 = load i64, ptr %4, align 8
	%12 = sext i64 %10 to i64
	%13 = sub i64 %12, %11
	%14 = trunc i64 %13 to i64
	store i64 %14, ptr %3, align 8
	%15 = load i64, ptr %3, align 8
	%16 = load i64, ptr %4, align 8
	%17 = sext i64 %15 to i64
	%18 = mul i64 %17, %16
	%19 = trunc i64 %18 to i64
	store i64 %19, ptr %3, align 8
	%20 = load i64, ptr %3, align 8
	%21 = load i64, ptr %4, align 8
	%22 = sext i64 %20 to i64
	%23 = udiv i64 %22, %21
	%24 = trunc i64 %23 to i64
	store i64 %24, ptr %3, align 8
	%25 = load i64, ptr %3, align 8
	%26 = load i64, ptr %4, align 8
	%27 = sext i64 %25 to i64
	%28 = add i64 %27, %26
	%29 = trunc i64 %28 to i64
	store i64 %29, ptr %3, align 8
	store i64 %-20000, ptr %2, align 8
	br label %label_1

label_1:
	%30 = load i64, ptr %2, align 8
	ret i64 %30
}

define dso_local i64 @test_ull_mixed() #0 {
	%2 = alloca i64, align 8
	%3 = alloca i64, align 8
	store i64 8000000000, ptr %3, align 8
	%4 = alloca i64, align 8
	store i64 -3000, ptr %4, align 8
	%5 = load i64, ptr %3, align 8
	%6 = load i64, ptr %4, align 8
	%7 = sext i64 %6 to i64
	%8 = add i64 %5, %7
	store i64 %8, ptr %3, align 8
	%9 = load i64, ptr %3, align 8
	%10 = load i64, ptr %4, align 8
	%11 = sext i64 %10 to i64
	%12 = sub i64 %9, %11
	store i64 %12, ptr %3, align 8
	%13 = load i64, ptr %3, align 8
	%14 = load i64, ptr %4, align 8
	%15 = sext i64 %14 to i64
	%16 = mul i64 %13, %15
	store i64 %16, ptr %3, align 8
	%17 = load i64, ptr %3, align 8
	%18 = load i64, ptr %4, align 8
	%19 = sext i64 %18 to i64
	%20 = udiv i64 %17, %19
	store i64 %20, ptr %3, align 8
	%21 = load i64, ptr %3, align 8
	%22 = load i64, ptr %4, align 8
	%23 = sext i64 %22 to i64
	%24 = add i64 %21, %23
	store i64 %24, ptr %3, align 8
	store i64 %-20000, ptr %2, align 8
	br label %label_1

label_1:
	%25 = load i64, ptr %2, align 8
	ret i64 %25
}

define dso_local double @test_double_int_mixed() #0 {
	%2 = alloca double, align 8
	%3 = alloca double, align 8
	store double 3.140000e+00, ptr %3, align 8
	%4 = alloca i32, align 4
	store i32 10, ptr %4, align 4
	%5 = alloca i32, align 4
	store i32 20, ptr %5, align 4
	%6 = load double, ptr %3, align 8
	%7 = load i32, ptr %4, align 4
	%8 = add void %0, %0
	%9 = load double, ptr %3, align 8
	%10 = load i32, ptr %5, align 4
	%11 = add void %0, %0
	%12 = load double, ptr %3, align 8
	%13 = load i32, ptr %4, align 4
	%14 = sub void %0, %0
	%15 = load double, ptr %3, align 8
	%16 = load i32, ptr %5, align 4
	%17 = mul void %0, %0
	%18 = load double, ptr %3, align 8
	%19 = load i32, ptr %4, align 4
	%20 = udiv void %0, %0
	%21 = load double, ptr %3, align 8
	%22 = load i32, ptr %4, align 4
	%23 = add void %0, %0
	%24 = load double, ptr %3, align 8
	%25 = load i32, ptr %5, align 4
	%26 = add void %0, %0
	store double %-20000, ptr %2, align 8
	br label %label_1

label_1:
	%27 = load double, ptr %2, align 8
	ret double %27
}

define dso_local float @test_float_int_mixed() #0 {
	%2 = alloca float, align 4
	%3 = alloca float, align 4
	store float 1.500000e+00, ptr %3, align 4
	%4 = alloca i32, align 4
	store i32 -3, ptr %4, align 4
	%5 = alloca i8, align 1
	store i8 -6, ptr %5, align 1
	%6 = load float, ptr %3, align 4
	%7 = load i32, ptr %4, align 4
	%8 = add void %0, %0
	%9 = load float, ptr %3, align 4
	%10 = load i8, ptr %5, align 1
	%11 = add void %0, %0
	%12 = load float, ptr %3, align 4
	%13 = load i32, ptr %4, align 4
	%14 = mul void %0, %0
	%15 = load float, ptr %3, align 4
	%16 = load i8, ptr %5, align 1
	%17 = udiv void %0, %0
	%18 = load float, ptr %3, align 4
	%19 = load i32, ptr %4, align 4
	%20 = add void %0, %0
	%21 = load float, ptr %3, align 4
	%22 = load i8, ptr %5, align 1
	%23 = add void %0, %0
	store float %-20000, ptr %2, align 4
	br label %label_1

label_1:
	%24 = load float, ptr %2, align 4
	ret float %24
}

define dso_local x86_fp80 @test_long_double_mixed() #0 {
	%2 = alloca x86_fp80, align 16
	%3 = alloca x86_fp80, align 16
	store x86_fp80 f0x3FFF9E04189374BC6B0B, ptr %3, align 16
	%4 = alloca i32, align 4
	store i32 7, ptr %4, align 4
	%5 = alloca i64, align 8
	store i64 -9999, ptr %5, align 8
	%6 = load x86_fp80, ptr %3, align 16
	%7 = load i32, ptr %4, align 4
	%8 = add void %0, %0
	%9 = load x86_fp80, ptr %3, align 16
	%10 = load i64, ptr %5, align 8
	%11 = add void %0, %0
	%12 = load x86_fp80, ptr %3, align 16
	%13 = load i32, ptr %4, align 4
	%14 = mul void %0, %0
	%15 = load x86_fp80, ptr %3, align 16
	%16 = load i64, ptr %5, align 8
	%17 = udiv void %0, %0
	%18 = load x86_fp80, ptr %3, align 16
	%19 = load i32, ptr %4, align 4
	%20 = add void %0, %0
	%21 = load x86_fp80, ptr %3, align 16
	%22 = load i64, ptr %5, align 8
	%23 = add void %0, %0
	store x86_fp80 %-20000, ptr %2, align 16
	br label %label_1

label_1:
	%24 = load x86_fp80, ptr %2, align 16
	ret x86_fp80 %24
}


attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic"}