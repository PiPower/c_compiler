

define dso_local i8 @test_signed_char() #0 {
	%1 = alloca i8, align 1
	%2 = alloca i8, align 1
	%3 = alloca i8, align 1
	store i8 100, ptr %2, align 1
	store i8 7, ptr %3, align 1
	%4 = load i8, ptr %2, align 1
	%5 = load i8, ptr %3, align 1
	%6 = sext i8 %4 to i32
	%7 = sext i8 %5 to i32
	%8 = add nsw i32 %6, %7
	%9 = add nsw i32 %8, 2
	%10 = add nsw i32 %9, 4
	%11 = trunc i32 %10 to i8
	store i8 %11, ptr %2, align 1
	%12 = load i8, ptr %2, align 1
	%13 = load i8, ptr %3, align 1
	%14 = sext i8 %12 to i32
	%15 = sext i8 %13 to i32
	%16 = sub nsw i32 %14, %15
	%17 = sub nsw i32 %16, 22
	%18 = trunc i32 %17 to i8
	store i8 %18, ptr %2, align 1
	%19 = load i8, ptr %2, align 1
	%20 = load i8, ptr %3, align 1
	%21 = sext i8 %19 to i32
	%22 = sext i8 %20 to i32
	%23 = mul nsw i32 %21, %22
	%24 = trunc i32 %23 to i8
	store i8 %24, ptr %2, align 1
	%25 = load i8, ptr %2, align 1
	%26 = load i8, ptr %3, align 1
	%27 = sext i8 %25 to i32
	%28 = sext i8 %26 to i32
	%29 = sdiv i32 %27, %28
	%30 = trunc i32 %29 to i8
	store i8 %30, ptr %2, align 1
	%31 = load i8, ptr %2, align 1
	%32 = load i8, ptr %3, align 1
	%33 = sext i8 %31 to i32
	%34 = sext i8 %32 to i32
	%35 = srem i32 %33, %34
	%36 = trunc i32 %35 to i8
	store i8 %36, ptr %2, align 1
	%37 = load i8, ptr %2, align 1
	%38 = load i8, ptr %3, align 1
	%39 = sext i8 %37 to i32
	%40 = sext i8 %38 to i32
	%41 = and i32 %39, %40
	%42 = trunc i32 %41 to i8
	store i8 %42, ptr %2, align 1
	%43 = load i8, ptr %2, align 1
	%44 = load i8, ptr %3, align 1
	%45 = sext i8 %43 to i32
	%46 = sext i8 %44 to i32
	%47 = or i32 %45, %46
	%48 = trunc i32 %47 to i8
	store i8 %48, ptr %2, align 1
	%49 = load i8, ptr %2, align 1
	%50 = load i8, ptr %3, align 1
	%51 = sext i8 %49 to i32
	%52 = sext i8 %50 to i32
	%53 = xor i32 %51, %52
	%54 = trunc i32 %53 to i8
	store i8 %54, ptr %2, align 1
	%55 = load i8, ptr %2, align 1
	%56 = sext i8 %55 to i32
	%57 = shl i32 %56, 2
	%58 = trunc i32 %57 to i8
	store i8 %58, ptr %2, align 1
	%59 = load i8, ptr %2, align 1
	%60 = sext i8 %59 to i32
	%61 = ashr i32 %60, 1
	%62 = trunc i32 %61 to i8
	store i8 %62, ptr %2, align 1
	%63 = load i8, ptr %2, align 1
	store i8 %63, ptr %1, align 1
	br label %label_1

label_1:
	%64 = load i8, ptr %1, align 1
	ret i8 %64
}

define dso_local i8 @test_unsigned_char() #0 {
	%1 = alloca i8, align 1
	%2 = alloca i8, align 1
	%3 = alloca i8, align 1
	store i8 -56, ptr %2, align 1
	store i8 13, ptr %3, align 1
	%4 = load i8, ptr %2, align 1
	%5 = load i8, ptr %3, align 1
	%6 = zext i8 %4 to i32
	%7 = zext i8 %5 to i32
	%8 = add nsw i32 %6, %7
	%9 = trunc i32 %8 to i8
	store i8 %9, ptr %2, align 1
	%10 = load i8, ptr %2, align 1
	%11 = load i8, ptr %3, align 1
	%12 = zext i8 %10 to i32
	%13 = zext i8 %11 to i32
	%14 = sub nsw i32 %12, %13
	%15 = trunc i32 %14 to i8
	store i8 %15, ptr %2, align 1
	%16 = load i8, ptr %2, align 1
	%17 = load i8, ptr %3, align 1
	%18 = zext i8 %16 to i32
	%19 = zext i8 %17 to i32
	%20 = mul nsw i32 %18, %19
	%21 = trunc i32 %20 to i8
	store i8 %21, ptr %2, align 1
	%22 = load i8, ptr %2, align 1
	%23 = load i8, ptr %3, align 1
	%24 = zext i8 %22 to i32
	%25 = zext i8 %23 to i32
	%26 = sdiv i32 %24, %25
	%27 = trunc i32 %26 to i8
	store i8 %27, ptr %2, align 1
	%28 = load i8, ptr %2, align 1
	%29 = load i8, ptr %3, align 1
	%30 = zext i8 %28 to i32
	%31 = zext i8 %29 to i32
	%32 = srem i32 %30, %31
	%33 = trunc i32 %32 to i8
	store i8 %33, ptr %2, align 1
	%34 = load i8, ptr %2, align 1
	%35 = load i8, ptr %3, align 1
	%36 = zext i8 %34 to i32
	%37 = zext i8 %35 to i32
	%38 = and i32 %36, %37
	%39 = trunc i32 %38 to i8
	store i8 %39, ptr %2, align 1
	%40 = load i8, ptr %2, align 1
	%41 = load i8, ptr %3, align 1
	%42 = zext i8 %40 to i32
	%43 = zext i8 %41 to i32
	%44 = or i32 %42, %43
	%45 = trunc i32 %44 to i8
	store i8 %45, ptr %2, align 1
	%46 = load i8, ptr %2, align 1
	%47 = load i8, ptr %3, align 1
	%48 = zext i8 %46 to i32
	%49 = zext i8 %47 to i32
	%50 = xor i32 %48, %49
	%51 = trunc i32 %50 to i8
	store i8 %51, ptr %2, align 1
	%52 = load i8, ptr %2, align 1
	%53 = zext i8 %52 to i32
	%54 = shl i32 %53, 2
	%55 = trunc i32 %54 to i8
	store i8 %55, ptr %2, align 1
	%56 = load i8, ptr %2, align 1
	%57 = zext i8 %56 to i32
	%58 = ashr i32 %57, 1
	%59 = trunc i32 %58 to i8
	store i8 %59, ptr %2, align 1
	%60 = load i8, ptr %2, align 1
	store i8 %60, ptr %1, align 1
	br label %label_1

label_1:
	%61 = load i8, ptr %1, align 1
	ret i8 %61
}

define dso_local i16 @test_short() #0 {
	%1 = alloca i16, align 2
	%2 = alloca i16, align 2
	%3 = alloca i16, align 2
	store i16 1234, ptr %2, align 2
	store i16 37, ptr %3, align 2
	%4 = load i16, ptr %2, align 2
	%5 = load i16, ptr %3, align 2
	%6 = sext i16 %4 to i32
	%7 = sext i16 %5 to i32
	%8 = add nsw i32 %6, %7
	%9 = trunc i32 %8 to i16
	store i16 %9, ptr %2, align 2
	%10 = load i16, ptr %2, align 2
	%11 = load i16, ptr %3, align 2
	%12 = sext i16 %10 to i32
	%13 = sext i16 %11 to i32
	%14 = sub nsw i32 %12, %13
	%15 = trunc i32 %14 to i16
	store i16 %15, ptr %2, align 2
	%16 = load i16, ptr %2, align 2
	%17 = load i16, ptr %3, align 2
	%18 = sext i16 %16 to i32
	%19 = sext i16 %17 to i32
	%20 = mul nsw i32 %18, %19
	%21 = trunc i32 %20 to i16
	store i16 %21, ptr %2, align 2
	%22 = load i16, ptr %2, align 2
	%23 = load i16, ptr %3, align 2
	%24 = sext i16 %22 to i32
	%25 = sext i16 %23 to i32
	%26 = sdiv i32 %24, %25
	%27 = trunc i32 %26 to i16
	store i16 %27, ptr %2, align 2
	%28 = load i16, ptr %2, align 2
	%29 = load i16, ptr %3, align 2
	%30 = sext i16 %28 to i32
	%31 = sext i16 %29 to i32
	%32 = srem i32 %30, %31
	%33 = trunc i32 %32 to i16
	store i16 %33, ptr %2, align 2
	%34 = load i16, ptr %2, align 2
	%35 = load i16, ptr %3, align 2
	%36 = sext i16 %34 to i32
	%37 = sext i16 %35 to i32
	%38 = and i32 %36, %37
	%39 = trunc i32 %38 to i16
	store i16 %39, ptr %2, align 2
	%40 = load i16, ptr %2, align 2
	%41 = load i16, ptr %3, align 2
	%42 = sext i16 %40 to i32
	%43 = sext i16 %41 to i32
	%44 = or i32 %42, %43
	%45 = trunc i32 %44 to i16
	store i16 %45, ptr %2, align 2
	%46 = load i16, ptr %2, align 2
	%47 = load i16, ptr %3, align 2
	%48 = sext i16 %46 to i32
	%49 = sext i16 %47 to i32
	%50 = xor i32 %48, %49
	%51 = trunc i32 %50 to i16
	store i16 %51, ptr %2, align 2
	%52 = load i16, ptr %2, align 2
	%53 = sext i16 %52 to i32
	%54 = shl i32 %53, 2
	%55 = trunc i32 %54 to i16
	store i16 %55, ptr %2, align 2
	%56 = load i16, ptr %2, align 2
	%57 = sext i16 %56 to i32
	%58 = ashr i32 %57, 1
	%59 = trunc i32 %58 to i16
	store i16 %59, ptr %2, align 2
	%60 = load i16, ptr %2, align 2
	store i16 %60, ptr %1, align 2
	br label %label_1

label_1:
	%61 = load i16, ptr %1, align 2
	ret i16 %61
}

define dso_local i16 @test_unsigned_short() #0 {
	%1 = alloca i16, align 2
	%2 = alloca i16, align 2
	%3 = alloca i16, align 2
	store i16 -11215, ptr %2, align 2
	store i16 123, ptr %3, align 2
	%4 = load i16, ptr %2, align 2
	%5 = load i16, ptr %3, align 2
	%6 = zext i16 %4 to i32
	%7 = zext i16 %5 to i32
	%8 = add nsw i32 %6, %7
	%9 = trunc i32 %8 to i16
	store i16 %9, ptr %2, align 2
	%10 = load i16, ptr %2, align 2
	%11 = load i16, ptr %3, align 2
	%12 = zext i16 %10 to i32
	%13 = zext i16 %11 to i32
	%14 = sub nsw i32 %12, %13
	%15 = trunc i32 %14 to i16
	store i16 %15, ptr %2, align 2
	%16 = load i16, ptr %2, align 2
	%17 = load i16, ptr %3, align 2
	%18 = zext i16 %16 to i32
	%19 = zext i16 %17 to i32
	%20 = mul nsw i32 %18, %19
	%21 = trunc i32 %20 to i16
	store i16 %21, ptr %2, align 2
	%22 = load i16, ptr %2, align 2
	%23 = load i16, ptr %3, align 2
	%24 = zext i16 %22 to i32
	%25 = zext i16 %23 to i32
	%26 = sdiv i32 %24, %25
	%27 = trunc i32 %26 to i16
	store i16 %27, ptr %2, align 2
	%28 = load i16, ptr %2, align 2
	%29 = load i16, ptr %3, align 2
	%30 = zext i16 %28 to i32
	%31 = zext i16 %29 to i32
	%32 = srem i32 %30, %31
	%33 = trunc i32 %32 to i16
	store i16 %33, ptr %2, align 2
	%34 = load i16, ptr %2, align 2
	%35 = load i16, ptr %3, align 2
	%36 = zext i16 %34 to i32
	%37 = zext i16 %35 to i32
	%38 = and i32 %36, %37
	%39 = trunc i32 %38 to i16
	store i16 %39, ptr %2, align 2
	%40 = load i16, ptr %2, align 2
	%41 = load i16, ptr %3, align 2
	%42 = zext i16 %40 to i32
	%43 = zext i16 %41 to i32
	%44 = or i32 %42, %43
	%45 = trunc i32 %44 to i16
	store i16 %45, ptr %2, align 2
	%46 = load i16, ptr %2, align 2
	%47 = load i16, ptr %3, align 2
	%48 = zext i16 %46 to i32
	%49 = zext i16 %47 to i32
	%50 = xor i32 %48, %49
	%51 = trunc i32 %50 to i16
	store i16 %51, ptr %2, align 2
	%52 = load i16, ptr %2, align 2
	%53 = zext i16 %52 to i32
	%54 = shl i32 %53, 2
	%55 = trunc i32 %54 to i16
	store i16 %55, ptr %2, align 2
	%56 = load i16, ptr %2, align 2
	%57 = zext i16 %56 to i32
	%58 = ashr i32 %57, 1
	%59 = trunc i32 %58 to i16
	store i16 %59, ptr %2, align 2
	%60 = load i16, ptr %2, align 2
	store i16 %60, ptr %1, align 2
	br label %label_1

label_1:
	%61 = load i16, ptr %1, align 2
	ret i16 %61
}

define dso_local i32 @test_int() #0 {
	%1 = alloca i32, align 4
	%2 = alloca i32, align 4
	%3 = alloca i32, align 4
	store i32 123456, ptr %2, align 4
	store i32 789, ptr %3, align 4
	%4 = load i32, ptr %2, align 4
	%5 = load i32, ptr %3, align 4
	%6 = add nsw i32 %4, %5
	store i32 %6, ptr %2, align 4
	%7 = load i32, ptr %2, align 4
	%8 = load i32, ptr %3, align 4
	%9 = sub nsw i32 %7, %8
	store i32 %9, ptr %2, align 4
	%10 = load i32, ptr %2, align 4
	%11 = load i32, ptr %3, align 4
	%12 = mul nsw i32 %10, %11
	store i32 %12, ptr %2, align 4
	%13 = load i32, ptr %2, align 4
	%14 = load i32, ptr %3, align 4
	%15 = sdiv i32 %13, %14
	store i32 %15, ptr %2, align 4
	%16 = load i32, ptr %2, align 4
	%17 = load i32, ptr %3, align 4
	%18 = srem i32 %16, %17
	store i32 %18, ptr %2, align 4
	%19 = load i32, ptr %2, align 4
	%20 = load i32, ptr %3, align 4
	%21 = and i32 %19, %20
	store i32 %21, ptr %2, align 4
	%22 = load i32, ptr %2, align 4
	%23 = load i32, ptr %3, align 4
	%24 = or i32 %22, %23
	store i32 %24, ptr %2, align 4
	%25 = load i32, ptr %2, align 4
	%26 = load i32, ptr %3, align 4
	%27 = xor i32 %25, %26
	store i32 %27, ptr %2, align 4
	%28 = load i32, ptr %2, align 4
	%29 = shl i32 %28, 3
	store i32 %29, ptr %2, align 4
	%30 = load i32, ptr %2, align 4
	%31 = ashr i32 %30, 2
	store i32 %31, ptr %2, align 4
	%32 = load i32, ptr %2, align 4
	store i32 %32, ptr %1, align 4
	br label %label_1

label_1:
	%33 = load i32, ptr %1, align 4
	ret i32 %33
}

define dso_local i32 @test_unsigned_int() #0 {
	%1 = alloca i32, align 4
	%2 = alloca i32, align 4
	%3 = alloca i32, align 4
	store i32 -294967296, ptr %2, align 4
	store i32 12345, ptr %3, align 4
	%4 = load i32, ptr %2, align 4
	%5 = load i32, ptr %3, align 4
	%6 = add i32 %4, %5
	store i32 %6, ptr %2, align 4
	%7 = load i32, ptr %2, align 4
	%8 = load i32, ptr %3, align 4
	%9 = sub i32 %7, %8
	store i32 %9, ptr %2, align 4
	%10 = load i32, ptr %2, align 4
	%11 = load i32, ptr %3, align 4
	%12 = mul i32 %10, %11
	store i32 %12, ptr %2, align 4
	%13 = load i32, ptr %2, align 4
	%14 = load i32, ptr %3, align 4
	%15 = udiv i32 %13, %14
	store i32 %15, ptr %2, align 4
	%16 = load i32, ptr %2, align 4
	%17 = load i32, ptr %3, align 4
	%18 = urem i32 %16, %17
	store i32 %18, ptr %2, align 4
	%19 = load i32, ptr %2, align 4
	%20 = load i32, ptr %3, align 4
	%21 = and i32 %19, %20
	store i32 %21, ptr %2, align 4
	%22 = load i32, ptr %2, align 4
	%23 = load i32, ptr %3, align 4
	%24 = or i32 %22, %23
	store i32 %24, ptr %2, align 4
	%25 = load i32, ptr %2, align 4
	%26 = load i32, ptr %3, align 4
	%27 = xor i32 %25, %26
	store i32 %27, ptr %2, align 4
	%28 = load i32, ptr %2, align 4
	%29 = shl i32 %28, 3
	store i32 %29, ptr %2, align 4
	%30 = load i32, ptr %2, align 4
	%31 = lshr i32 %30, 2
	store i32 %31, ptr %2, align 4
	%32 = load i32, ptr %2, align 4
	store i32 %32, ptr %1, align 4
	br label %label_1

label_1:
	%33 = load i32, ptr %1, align 4
	ret i32 %33
}

define dso_local i64 @test_long() #0 {
	%1 = alloca i64, align 8
	%2 = alloca i64, align 8
	%3 = alloca i64, align 8
	store i64 123456789, ptr %2, align 8
	store i64 1234, ptr %3, align 8
	%4 = load i64, ptr %2, align 8
	%5 = load i64, ptr %3, align 8
	%6 = add nsw i64 %4, %5
	store i64 %6, ptr %2, align 8
	%7 = load i64, ptr %2, align 8
	%8 = load i64, ptr %3, align 8
	%9 = sub nsw i64 %7, %8
	store i64 %9, ptr %2, align 8
	%10 = load i64, ptr %2, align 8
	%11 = load i64, ptr %3, align 8
	%12 = mul nsw i64 %10, %11
	store i64 %12, ptr %2, align 8
	%13 = load i64, ptr %2, align 8
	%14 = load i64, ptr %3, align 8
	%15 = sdiv i64 %13, %14
	store i64 %15, ptr %2, align 8
	%16 = load i64, ptr %2, align 8
	%17 = load i64, ptr %3, align 8
	%18 = srem i64 %16, %17
	store i64 %18, ptr %2, align 8
	%19 = load i64, ptr %2, align 8
	%20 = load i64, ptr %3, align 8
	%21 = and i64 %19, %20
	store i64 %21, ptr %2, align 8
	%22 = load i64, ptr %2, align 8
	%23 = load i64, ptr %3, align 8
	%24 = or i64 %22, %23
	store i64 %24, ptr %2, align 8
	%25 = load i64, ptr %2, align 8
	%26 = load i64, ptr %3, align 8
	%27 = xor i64 %25, %26
	store i64 %27, ptr %2, align 8
	%28 = load i64, ptr %2, align 8
	%29 = shl i64 %28, 4
	store i64 %29, ptr %2, align 8
	%30 = load i64, ptr %2, align 8
	%31 = ashr i64 %30, 3
	store i64 %31, ptr %2, align 8
	%32 = load i64, ptr %2, align 8
	store i64 %32, ptr %1, align 8
	br label %label_1

label_1:
	%33 = load i64, ptr %1, align 8
	ret i64 %33
}

define dso_local i64 @test_unsigned_long() #0 {
	%1 = alloca i64, align 8
	%2 = alloca i64, align 8
	%3 = alloca i64, align 8
	store i64 3000000000, ptr %2, align 8
	store i64 777, ptr %3, align 8
	%4 = load i64, ptr %2, align 8
	%5 = load i64, ptr %3, align 8
	%6 = add i64 %4, %5
	store i64 %6, ptr %2, align 8
	%7 = load i64, ptr %2, align 8
	%8 = load i64, ptr %3, align 8
	%9 = sub i64 %7, %8
	store i64 %9, ptr %2, align 8
	%10 = load i64, ptr %2, align 8
	%11 = load i64, ptr %3, align 8
	%12 = mul i64 %10, %11
	store i64 %12, ptr %2, align 8
	%13 = load i64, ptr %2, align 8
	%14 = load i64, ptr %3, align 8
	%15 = udiv i64 %13, %14
	store i64 %15, ptr %2, align 8
	%16 = load i64, ptr %2, align 8
	%17 = load i64, ptr %3, align 8
	%18 = urem i64 %16, %17
	store i64 %18, ptr %2, align 8
	%19 = load i64, ptr %2, align 8
	%20 = load i64, ptr %3, align 8
	%21 = and i64 %19, %20
	store i64 %21, ptr %2, align 8
	%22 = load i64, ptr %2, align 8
	%23 = load i64, ptr %3, align 8
	%24 = or i64 %22, %23
	store i64 %24, ptr %2, align 8
	%25 = load i64, ptr %2, align 8
	%26 = load i64, ptr %3, align 8
	%27 = xor i64 %25, %26
	store i64 %27, ptr %2, align 8
	%28 = load i64, ptr %2, align 8
	%29 = shl i64 %28, 4
	store i64 %29, ptr %2, align 8
	%30 = load i64, ptr %2, align 8
	%31 = lshr i64 %30, 3
	store i64 %31, ptr %2, align 8
	%32 = load i64, ptr %2, align 8
	store i64 %32, ptr %1, align 8
	br label %label_1

label_1:
	%33 = load i64, ptr %1, align 8
	ret i64 %33
}

define dso_local i64 @test_long_long() #0 {
	%1 = alloca i64, align 8
	%2 = alloca i64, align 8
	%3 = alloca i64, align 8
	store i64 1234567890123, ptr %2, align 8
	store i64 4567, ptr %3, align 8
	%4 = load i64, ptr %2, align 8
	%5 = load i64, ptr %3, align 8
	%6 = add nsw i64 %4, %5
	store i64 %6, ptr %2, align 8
	%7 = load i64, ptr %2, align 8
	%8 = load i64, ptr %3, align 8
	%9 = sub nsw i64 %7, %8
	store i64 %9, ptr %2, align 8
	%10 = load i64, ptr %2, align 8
	%11 = load i64, ptr %3, align 8
	%12 = mul nsw i64 %10, %11
	store i64 %12, ptr %2, align 8
	%13 = load i64, ptr %2, align 8
	%14 = load i64, ptr %3, align 8
	%15 = sdiv i64 %13, %14
	store i64 %15, ptr %2, align 8
	%16 = load i64, ptr %2, align 8
	%17 = load i64, ptr %3, align 8
	%18 = srem i64 %16, %17
	store i64 %18, ptr %2, align 8
	%19 = load i64, ptr %2, align 8
	%20 = load i64, ptr %3, align 8
	%21 = and i64 %19, %20
	store i64 %21, ptr %2, align 8
	%22 = load i64, ptr %2, align 8
	%23 = load i64, ptr %3, align 8
	%24 = or i64 %22, %23
	store i64 %24, ptr %2, align 8
	%25 = load i64, ptr %2, align 8
	%26 = load i64, ptr %3, align 8
	%27 = xor i64 %25, %26
	store i64 %27, ptr %2, align 8
	%28 = load i64, ptr %2, align 8
	%29 = shl i64 %28, 5
	store i64 %29, ptr %2, align 8
	%30 = load i64, ptr %2, align 8
	%31 = ashr i64 %30, 4
	store i64 %31, ptr %2, align 8
	%32 = load i64, ptr %2, align 8
	store i64 %32, ptr %1, align 8
	br label %label_1

label_1:
	%33 = load i64, ptr %1, align 8
	ret i64 %33
}

define dso_local i64 @test_unsigned_long_long() #0 {
	%1 = alloca i64, align 8
	%2 = alloca i64, align 8
	%3 = alloca i64, align 8
	store i64 123456789012345, ptr %2, align 8
	store i64 98765, ptr %3, align 8
	%4 = load i64, ptr %2, align 8
	%5 = load i64, ptr %3, align 8
	%6 = add i64 %4, %5
	store i64 %6, ptr %2, align 8
	%7 = load i64, ptr %2, align 8
	%8 = load i64, ptr %3, align 8
	%9 = sub i64 %7, %8
	store i64 %9, ptr %2, align 8
	%10 = load i64, ptr %2, align 8
	%11 = load i64, ptr %3, align 8
	%12 = mul i64 %10, %11
	store i64 %12, ptr %2, align 8
	%13 = load i64, ptr %2, align 8
	%14 = load i64, ptr %3, align 8
	%15 = udiv i64 %13, %14
	store i64 %15, ptr %2, align 8
	%16 = load i64, ptr %2, align 8
	%17 = load i64, ptr %3, align 8
	%18 = urem i64 %16, %17
	store i64 %18, ptr %2, align 8
	%19 = load i64, ptr %2, align 8
	%20 = load i64, ptr %3, align 8
	%21 = and i64 %19, %20
	store i64 %21, ptr %2, align 8
	%22 = load i64, ptr %2, align 8
	%23 = load i64, ptr %3, align 8
	%24 = or i64 %22, %23
	store i64 %24, ptr %2, align 8
	%25 = load i64, ptr %2, align 8
	%26 = load i64, ptr %3, align 8
	%27 = xor i64 %25, %26
	store i64 %27, ptr %2, align 8
	%28 = load i64, ptr %2, align 8
	%29 = shl i64 %28, 5
	store i64 %29, ptr %2, align 8
	%30 = load i64, ptr %2, align 8
	%31 = lshr i64 %30, 4
	store i64 %31, ptr %2, align 8
	%32 = load i64, ptr %2, align 8
	store i64 %32, ptr %1, align 8
	br label %label_1

label_1:
	%33 = load i64, ptr %1, align 8
	ret i64 %33
}

define dso_local float @test_float() #0 {
	%1 = alloca float, align 4
	%2 = alloca float, align 4
	%3 = alloca float, align 4
	store float 0x400C000000000000, ptr %2, align 4
	store float 0x3FF4000000000000, ptr %3, align 4
	%4 = load float, ptr %2, align 4
	%5 = load float, ptr %3, align 4
	%6 = fadd float %4, %5
	store float %6, ptr %2, align 4
	%7 = load float, ptr %2, align 4
	%8 = load float, ptr %3, align 4
	%9 = fsub float %7, %8
	store float %9, ptr %2, align 4
	%10 = load float, ptr %2, align 4
	%11 = load float, ptr %3, align 4
	%12 = fmul float %10, %11
	store float %12, ptr %2, align 4
	%13 = load float, ptr %2, align 4
	%14 = load float, ptr %3, align 4
	%15 = fdiv float %13, %14
	store float %15, ptr %2, align 4
	%16 = load float, ptr %2, align 4
	store float %16, ptr %1, align 4
	br label %label_1

label_1:
	%17 = load float, ptr %1, align 4
	ret float %17
}

define dso_local double @test_double() #0 {
	%1 = alloca double, align 8
	%2 = alloca double, align 8
	%3 = alloca double, align 8
	store double 0x400921FB54442D18, ptr %2, align 8
	store double 0x4005BF0A8B145769, ptr %3, align 8
	%4 = load double, ptr %2, align 8
	%5 = load double, ptr %3, align 8
	%6 = fadd double %4, %5
	store double %6, ptr %2, align 8
	%7 = load double, ptr %2, align 8
	%8 = load double, ptr %3, align 8
	%9 = fsub double %7, %8
	store double %9, ptr %2, align 8
	%10 = load double, ptr %2, align 8
	%11 = load double, ptr %3, align 8
	%12 = fmul double %10, %11
	store double %12, ptr %2, align 8
	%13 = load double, ptr %2, align 8
	%14 = load double, ptr %3, align 8
	%15 = fdiv double %13, %14
	store double %15, ptr %2, align 8
	%16 = load double, ptr %2, align 8
	store double %16, ptr %1, align 8
	br label %label_1

label_1:
	%17 = load double, ptr %1, align 8
	ret double %17
}

define dso_local x86_fp80 @test_long_double() #0 {
	%1 = alloca x86_fp80, align 16
	%2 = alloca x86_fp80, align 16
	%3 = alloca x86_fp80, align 16
	store x86_fp80 0xK3FFF9E06521462CFDC1A, ptr %2, align 16
	store x86_fp80 0xK40029E06522C8B909A4C, ptr %3, align 16
	%4 = load x86_fp80, ptr %2, align 16
	%5 = load x86_fp80, ptr %3, align 16
	%6 = fadd x86_fp80 %4, %5
	store x86_fp80 %6, ptr %2, align 16
	%7 = load x86_fp80, ptr %2, align 16
	%8 = load x86_fp80, ptr %3, align 16
	%9 = fsub x86_fp80 %7, %8
	store x86_fp80 %9, ptr %2, align 16
	%10 = load x86_fp80, ptr %2, align 16
	%11 = load x86_fp80, ptr %3, align 16
	%12 = fmul x86_fp80 %10, %11
	store x86_fp80 %12, ptr %2, align 16
	%13 = load x86_fp80, ptr %2, align 16
	%14 = load x86_fp80, ptr %3, align 16
	%15 = fdiv x86_fp80 %13, %14
	store x86_fp80 %15, ptr %2, align 16
	%16 = load x86_fp80, ptr %2, align 16
	store x86_fp80 %16, ptr %1, align 16
	br label %label_1

label_1:
	%17 = load x86_fp80, ptr %1, align 16
	ret x86_fp80 %17
}

define dso_local i32 @test_int_mixed() #0 {
	%1 = alloca i32, align 4
	%2 = alloca i32, align 4
	%3 = alloca i32, align 4
	store i32 1000, ptr %2, align 4
	store i32 4000, ptr %3, align 4
	%4 = load i32, ptr %2, align 4
	%5 = load i32, ptr %3, align 4
	%6 = add i32 %4, %5
	store i32 %6, ptr %2, align 4
	%7 = load i32, ptr %2, align 4
	%8 = load i32, ptr %3, align 4
	%9 = sub i32 %7, %8
	store i32 %9, ptr %2, align 4
	%10 = load i32, ptr %2, align 4
	%11 = load i32, ptr %3, align 4
	%12 = mul i32 %10, %11
	store i32 %12, ptr %2, align 4
	%13 = load i32, ptr %2, align 4
	%14 = load i32, ptr %3, align 4
	%15 = udiv i32 %13, %14
	store i32 %15, ptr %2, align 4
	%16 = load i32, ptr %2, align 4
	%17 = load i32, ptr %3, align 4
	%18 = add i32 %16, %17
	store i32 %18, ptr %2, align 4
	%19 = load i32, ptr %2, align 4
	%20 = load i32, ptr %3, align 4
	%21 = sub i32 %19, %20
	store i32 %21, ptr %2, align 4
	%22 = load i32, ptr %2, align 4
	store i32 %22, ptr %1, align 4
	br label %label_1

label_1:
	%23 = load i32, ptr %1, align 4
	ret i32 %23
}

define dso_local i32 @test_unsigned_int_mixed() #0 {
	%1 = alloca i32, align 4
	%2 = alloca i32, align 4
	%3 = alloca i32, align 4
	store i32 3000, ptr %2, align 4
	store i32 -1200, ptr %3, align 4
	%4 = load i32, ptr %2, align 4
	%5 = load i32, ptr %3, align 4
	%6 = add i32 %4, %5
	store i32 %6, ptr %2, align 4
	%7 = load i32, ptr %2, align 4
	%8 = load i32, ptr %3, align 4
	%9 = sub i32 %7, %8
	store i32 %9, ptr %2, align 4
	%10 = load i32, ptr %2, align 4
	%11 = load i32, ptr %3, align 4
	%12 = mul i32 %10, %11
	store i32 %12, ptr %2, align 4
	%13 = load i32, ptr %2, align 4
	%14 = load i32, ptr %3, align 4
	%15 = udiv i32 %13, %14
	store i32 %15, ptr %2, align 4
	%16 = load i32, ptr %2, align 4
	%17 = load i32, ptr %3, align 4
	%18 = add i32 %16, %17
	store i32 %18, ptr %2, align 4
	%19 = load i32, ptr %2, align 4
	store i32 %19, ptr %1, align 4
	br label %label_1

label_1:
	%20 = load i32, ptr %1, align 4
	ret i32 %20
}

define dso_local i32 @test_char_short_mixed() #0 {
	%1 = alloca i32, align 4
	%2 = alloca i8, align 1
	%3 = alloca i8, align 1
	%4 = alloca i16, align 2
	%5 = alloca i16, align 2
	%6 = alloca i32, align 4
	store i8 -10, ptr %2, align 1
	store i8 -56, ptr %3, align 1
	store i16 3000, ptr %4, align 2
	store i16 -5536, ptr %5, align 2
	%7 = load i8, ptr %2, align 1
	%8 = sext i8 %7 to i32
	store i32 %8, ptr %6, align 4
	%9 = load i32, ptr %6, align 4
	%10 = load i8, ptr %3, align 1
	%11 = zext i8 %10 to i32
	%12 = add nsw i32 %9, %11
	store i32 %12, ptr %6, align 4
	%13 = load i32, ptr %6, align 4
	%14 = load i16, ptr %4, align 2
	%15 = sext i16 %14 to i32
	%16 = add nsw i32 %13, %15
	store i32 %16, ptr %6, align 4
	%17 = load i32, ptr %6, align 4
	%18 = load i16, ptr %5, align 2
	%19 = zext i16 %18 to i32
	%20 = add nsw i32 %17, %19
	store i32 %20, ptr %6, align 4
	%21 = load i32, ptr %6, align 4
	%22 = load i8, ptr %2, align 1
	%23 = load i8, ptr %3, align 1
	%24 = sext i8 %22 to i32
	%25 = zext i8 %23 to i32
	%26 = add nsw i32 %24, %25
	%27 = add nsw i32 %21, %26
	store i32 %27, ptr %6, align 4
	%28 = load i32, ptr %6, align 4
	%29 = load i16, ptr %4, align 2
	%30 = load i16, ptr %5, align 2
	%31 = sext i16 %29 to i32
	%32 = zext i16 %30 to i32
	%33 = add nsw i32 %31, %32
	%34 = add nsw i32 %28, %33
	store i32 %34, ptr %6, align 4
	%35 = load i32, ptr %6, align 4
	store i32 %35, ptr %1, align 4
	br label %label_1

label_1:
	%36 = load i32, ptr %1, align 4
	ret i32 %36
}

define dso_local i64 @test_long_int_mixed() #0 {
	%1 = alloca i64, align 8
	%2 = alloca i64, align 8
	%3 = alloca i32, align 4
	store i64 100000, ptr %2, align 8
	store i32 -500, ptr %3, align 4
	%4 = load i64, ptr %2, align 8
	%5 = load i32, ptr %3, align 4
	%6 = sext i32 %5 to i64
	%7 = add nsw i64 %4, %6
	store i64 %7, ptr %2, align 8
	%8 = load i64, ptr %2, align 8
	%9 = load i32, ptr %3, align 4
	%10 = sext i32 %9 to i64
	%11 = sub nsw i64 %8, %10
	store i64 %11, ptr %2, align 8
	%12 = load i64, ptr %2, align 8
	%13 = load i32, ptr %3, align 4
	%14 = sext i32 %13 to i64
	%15 = mul nsw i64 %12, %14
	store i64 %15, ptr %2, align 8
	%16 = load i64, ptr %2, align 8
	%17 = load i32, ptr %3, align 4
	%18 = sext i32 %17 to i64
	%19 = sdiv i64 %16, %18
	store i64 %19, ptr %2, align 8
	%20 = load i64, ptr %2, align 8
	%21 = load i32, ptr %3, align 4
	%22 = sext i32 %21 to i64
	%23 = add nsw i64 %20, %22
	store i64 %23, ptr %2, align 8
	%24 = load i64, ptr %2, align 8
	store i64 %24, ptr %1, align 8
	br label %label_1

label_1:
	%25 = load i64, ptr %1, align 8
	ret i64 %25
}

define dso_local i64 @test_ulong_int_mixed() #0 {
	%1 = alloca i64, align 8
	%2 = alloca i64, align 8
	%3 = alloca i32, align 4
	store i64 3000000000, ptr %2, align 8
	store i32 -1000, ptr %3, align 4
	%4 = load i64, ptr %2, align 8
	%5 = load i32, ptr %3, align 4
	%6 = sext i32 %5 to i64
	%7 = add i64 %4, %6
	store i64 %7, ptr %2, align 8
	%8 = load i64, ptr %2, align 8
	%9 = load i32, ptr %3, align 4
	%10 = sext i32 %9 to i64
	%11 = sub i64 %8, %10
	store i64 %11, ptr %2, align 8
	%12 = load i64, ptr %2, align 8
	%13 = load i32, ptr %3, align 4
	%14 = sext i32 %13 to i64
	%15 = mul i64 %12, %14
	store i64 %15, ptr %2, align 8
	%16 = load i64, ptr %2, align 8
	%17 = load i32, ptr %3, align 4
	%18 = sext i32 %17 to i64
	%19 = udiv i64 %16, %18
	store i64 %19, ptr %2, align 8
	%20 = load i64, ptr %2, align 8
	%21 = load i32, ptr %3, align 4
	%22 = sext i32 %21 to i64
	%23 = add i64 %20, %22
	store i64 %23, ptr %2, align 8
	%24 = load i64, ptr %2, align 8
	store i64 %24, ptr %1, align 8
	br label %label_1

label_1:
	%25 = load i64, ptr %1, align 8
	ret i64 %25
}

define dso_local i64 @test_ll_mixed() #0 {
	%1 = alloca i64, align 8
	%2 = alloca i64, align 8
	%3 = alloca i64, align 8
	store i64 5000000000, ptr %2, align 8
	store i64 9000000000, ptr %3, align 8
	%4 = load i64, ptr %2, align 8
	%5 = load i64, ptr %3, align 8
	%6 = add i64 %4, %5
	store i64 %6, ptr %2, align 8
	%7 = load i64, ptr %2, align 8
	%8 = load i64, ptr %3, align 8
	%9 = sub i64 %7, %8
	store i64 %9, ptr %2, align 8
	%10 = load i64, ptr %2, align 8
	%11 = load i64, ptr %3, align 8
	%12 = mul i64 %10, %11
	store i64 %12, ptr %2, align 8
	%13 = load i64, ptr %2, align 8
	%14 = load i64, ptr %3, align 8
	%15 = udiv i64 %13, %14
	store i64 %15, ptr %2, align 8
	%16 = load i64, ptr %2, align 8
	%17 = load i64, ptr %3, align 8
	%18 = add i64 %16, %17
	store i64 %18, ptr %2, align 8
	%19 = load i64, ptr %2, align 8
	store i64 %19, ptr %1, align 8
	br label %label_1

label_1:
	%20 = load i64, ptr %1, align 8
	ret i64 %20
}

define dso_local i64 @test_ull_mixed() #0 {
	%1 = alloca i64, align 8
	%2 = alloca i64, align 8
	%3 = alloca i64, align 8
	store i64 8000000000, ptr %2, align 8
	store i64 -3000, ptr %3, align 8
	%4 = load i64, ptr %2, align 8
	%5 = load i64, ptr %3, align 8
	%6 = add i64 %4, %5
	store i64 %6, ptr %2, align 8
	%7 = load i64, ptr %2, align 8
	%8 = load i64, ptr %3, align 8
	%9 = sub i64 %7, %8
	store i64 %9, ptr %2, align 8
	%10 = load i64, ptr %2, align 8
	%11 = load i64, ptr %3, align 8
	%12 = mul i64 %10, %11
	store i64 %12, ptr %2, align 8
	%13 = load i64, ptr %2, align 8
	%14 = load i64, ptr %3, align 8
	%15 = udiv i64 %13, %14
	store i64 %15, ptr %2, align 8
	%16 = load i64, ptr %2, align 8
	%17 = load i64, ptr %3, align 8
	%18 = add i64 %16, %17
	store i64 %18, ptr %2, align 8
	%19 = load i64, ptr %2, align 8
	store i64 %19, ptr %1, align 8
	br label %label_1

label_1:
	%20 = load i64, ptr %1, align 8
	ret i64 %20
}

define dso_local double @test_double_int_mixed() #0 {
	%1 = alloca double, align 8
	%2 = alloca double, align 8
	%3 = alloca i32, align 4
	%4 = alloca i32, align 4
	store double 0x40091EB851EB851F, ptr %2, align 8
	store i32 10, ptr %3, align 4
	store i32 20, ptr %4, align 4
	%5 = load double, ptr %2, align 8
	%6 = load i32, ptr %3, align 4
	%7 = sitofp i32 %6 to double
	%8 = fadd double %5, %7
	store double %8, ptr %2, align 8
	%9 = load double, ptr %2, align 8
	%10 = load i32, ptr %4, align 4
	%11 = uitofp i32 %10 to double
	%12 = fadd double %9, %11
	store double %12, ptr %2, align 8
	%13 = load double, ptr %2, align 8
	%14 = load i32, ptr %3, align 4
	%15 = sitofp i32 %14 to double
	%16 = fsub double %13, %15
	store double %16, ptr %2, align 8
	%17 = load double, ptr %2, align 8
	%18 = load i32, ptr %4, align 4
	%19 = uitofp i32 %18 to double
	%20 = fmul double %17, %19
	store double %20, ptr %2, align 8
	%21 = load double, ptr %2, align 8
	%22 = load i32, ptr %3, align 4
	%23 = sitofp i32 %22 to double
	%24 = fdiv double %21, %23
	store double %24, ptr %2, align 8
	%25 = load double, ptr %2, align 8
	%26 = load i32, ptr %3, align 4
	%27 = sitofp i32 %26 to double
	%28 = fadd double %25, %27
	store double %28, ptr %2, align 8
	%29 = load double, ptr %2, align 8
	%30 = load i32, ptr %4, align 4
	%31 = uitofp i32 %30 to double
	%32 = fadd double %29, %31
	store double %32, ptr %2, align 8
	%33 = load double, ptr %2, align 8
	store double %33, ptr %1, align 8
	br label %label_1

label_1:
	%34 = load double, ptr %1, align 8
	ret double %34
}

define dso_local float @test_float_int_mixed() #0 {
	%1 = alloca float, align 4
	%2 = alloca float, align 4
	%3 = alloca i32, align 4
	%4 = alloca i8, align 1
	store float 0x3FF8000000000000, ptr %2, align 4
	store i32 -3, ptr %3, align 4
	store i8 -6, ptr %4, align 1
	%5 = load float, ptr %2, align 4
	%6 = load i32, ptr %3, align 4
	%7 = sitofp i32 %6 to float
	%8 = fadd float %5, %7
	store float %8, ptr %2, align 4
	%9 = load float, ptr %2, align 4
	%10 = load i8, ptr %4, align 1
	%11 = zext i8 %10 to i32
	%12 = sitofp i32 %11 to float
	%13 = fadd float %9, %12
	store float %13, ptr %2, align 4
	%14 = load float, ptr %2, align 4
	%15 = load i32, ptr %3, align 4
	%16 = sitofp i32 %15 to float
	%17 = fmul float %14, %16
	store float %17, ptr %2, align 4
	%18 = load float, ptr %2, align 4
	%19 = load i8, ptr %4, align 1
	%20 = zext i8 %19 to i32
	%21 = sitofp i32 %20 to float
	%22 = fdiv float %18, %21
	store float %22, ptr %2, align 4
	%23 = load float, ptr %2, align 4
	%24 = load i32, ptr %3, align 4
	%25 = sitofp i32 %24 to float
	%26 = fadd float %23, %25
	store float %26, ptr %2, align 4
	%27 = load float, ptr %2, align 4
	%28 = load i8, ptr %4, align 1
	%29 = zext i8 %28 to i32
	%30 = sitofp i32 %29 to float
	%31 = fadd float %27, %30
	store float %31, ptr %2, align 4
	%32 = load float, ptr %2, align 4
	store float %32, ptr %1, align 4
	br label %label_1

label_1:
	%33 = load float, ptr %1, align 4
	ret float %33
}

define dso_local x86_fp80 @test_long_double_mixed() #0 {
	%1 = alloca x86_fp80, align 16
	%2 = alloca x86_fp80, align 16
	%3 = alloca i32, align 4
	%4 = alloca i64, align 8
	store x86_fp80 0xK3FFF9E04189374BC6B0B, ptr %2, align 16
	store i32 7, ptr %3, align 4
	store i64 -9999, ptr %4, align 8
	%5 = load x86_fp80, ptr %2, align 16
	%6 = load i32, ptr %3, align 4
	%7 = sitofp i32 %6 to x86_fp80
	%8 = fadd x86_fp80 %5, %7
	store x86_fp80 %8, ptr %2, align 16
	%9 = load x86_fp80, ptr %2, align 16
	%10 = load i64, ptr %4, align 8
	%11 = sitofp i64 %10 to x86_fp80
	%12 = fadd x86_fp80 %9, %11
	store x86_fp80 %12, ptr %2, align 16
	%13 = load x86_fp80, ptr %2, align 16
	%14 = load i32, ptr %3, align 4
	%15 = sitofp i32 %14 to x86_fp80
	%16 = fmul x86_fp80 %13, %15
	store x86_fp80 %16, ptr %2, align 16
	%17 = load x86_fp80, ptr %2, align 16
	%18 = load i64, ptr %4, align 8
	%19 = sitofp i64 %18 to x86_fp80
	%20 = fdiv x86_fp80 %17, %19
	store x86_fp80 %20, ptr %2, align 16
	%21 = load x86_fp80, ptr %2, align 16
	%22 = load i32, ptr %3, align 4
	%23 = sitofp i32 %22 to x86_fp80
	%24 = fadd x86_fp80 %21, %23
	store x86_fp80 %24, ptr %2, align 16
	%25 = load x86_fp80, ptr %2, align 16
	%26 = load i64, ptr %4, align 8
	%27 = sitofp i64 %26 to x86_fp80
	%28 = fadd x86_fp80 %25, %27
	store x86_fp80 %28, ptr %2, align 16
	%29 = load x86_fp80, ptr %2, align 16
	store x86_fp80 %29, ptr %1, align 16
	br label %label_1

label_1:
	%30 = load x86_fp80, ptr %1, align 16
	ret x86_fp80 %30
}


attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic"}