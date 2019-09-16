stock SetupProducts()
{
	// Jedzenie
	InitProduct(ORDER_FOOD, ITEM_TYPE_FOOD, 2766, "Odnawiaj¹ce 5% HP", "", 5, 0, 1);
	InitProduct(ORDER_FOOD, ITEM_TYPE_FOOD, 2766, "Odnawiaj¹ce 10% HP", "", 10, 0, 3);
	InitProduct(ORDER_FOOD, ITEM_TYPE_FOOD, 2766, "Odnawiaj¹ce 20% HP", "", 20, 0, 5);
	InitProduct(ORDER_FOOD, ITEM_TYPE_FOOD, 2766, "Odnawiaj¹ce 30% HP", "", 30, 0, 8);
	InitProduct(ORDER_FOOD, ITEM_TYPE_FOOD, 2766, "Odnawiaj¹ce 50% HP", "", 50, 0, 12);
	
	// Napoje
	InitProduct(ORDER_DRINK, ITEM_TYPE_DRINK, 1546, "Odnawiaj¹ce 5% HP", "", 5, 0, 1);
	InitProduct(ORDER_DRINK, ITEM_TYPE_DRINK, 1546, "Odnawiaj¹ce 10% HP", "", 10, 0, 3);
	
	// Alkohole
	InitProduct(ORDER_BEVERAGE, ITEM_TYPE_DRINK, 1543, "S³aby alkohol", "", 0, 4, 10);
	InitProduct(ORDER_BEVERAGE, ITEM_TYPE_DRINK, 1512, "Mocny alokhol", "", 0, 9, 20);
	
	// Telefony
	InitProduct(ORDER_PHONES, ITEM_TYPE_PHONE, 18874, "Samsung Galaxy S2", "Samsung Galaxy S2", 0, 0, 40);
	
	// Shity
	InitProduct(ORDER_SHITS, ITEM_TYPE_NOTEPAD, 2894, "Zeszyt 10 kartkowy", "Ma³y zeszyt", 10, 0, 2);
	InitProduct(ORDER_SHITS, ITEM_TYPE_NOTEPAD, 2894, "Zeszyt 20 kartkowy", "Œredni zeszyt", 10, 0, 4);
	InitProduct(ORDER_SHITS, ITEM_TYPE_NOTEPAD, 2894, "Zeszyt 50 kartkowy", "Du¿y zeszyt", 10, 0, 6);
	
	// Gang
	InitProduct(ORDER_GANG, ITEM_TYPE_WEAPON, 331, "Kastet", "Kastet", 1, 1, 75);
	InitProduct(ORDER_GANG, ITEM_TYPE_WEAPON, 335, "Nó¿", "Nó¿", 4, 1, 150);
	InitProduct(ORDER_GANG, ITEM_TYPE_WEAPON, 336, "Kij baseballowy", "Kij baseballowy", 5, 1, 100);
	InitProduct(ORDER_GANG, ITEM_TYPE_MASK, 19036, "Bandana", "Bandana", 20, 0, 500);
	InitProduct(ORDER_GANG, ITEM_TYPE_ACCESSORY, 18911, "Bandana w czaszki", "[A] Bandana w czaszki", 0, 2, 50);
	InitProduct(ORDER_GANG, ITEM_TYPE_ACCESSORY, 18912, "Czarna bandana", "[A] Czarna bandana", 0, 2, 50);
	InitProduct(ORDER_GANG, ITEM_TYPE_ACCESSORY, 18913, "Zielona bandana", "[A] Zielona bandana", 0, 2, 50);
	InitProduct(ORDER_GANG, ITEM_TYPE_ACCESSORY, 18914, "Bandana moro", "[A] Bandana moro", 0, 2, 50);
	InitProduct(ORDER_GANG, ITEM_TYPE_ACCESSORY, 18915, "Ró¿owa bandana", "[A] Rozowa bandana", 0, 2, 50);
	InitProduct(ORDER_GANG, ITEM_TYPE_ACCESSORY, 18916, "¯ó³ta bandana", "[A] Zolta bandana", 0, 2, 50);
	InitProduct(ORDER_GANG, ITEM_TYPE_ACCESSORY, 18917, "Niebieska bandana", "[A] Niebieska bandana", 0, 2, 50);
	InitProduct(ORDER_GANG, ITEM_TYPE_ACCESSORY, 18918, "Czarno-bia³a bandana", "[A] Czarno-biala bandana", 0, 2, 50);
	InitProduct(ORDER_GANG, ITEM_TYPE_ACCESSORY, 18919, "Bandana w kropki", "[A] Bandana w kropki", 0, 2, 50);
	InitProduct(ORDER_GANG, ITEM_TYPE_ACCESSORY, 18920, "¯ó³to-br¹zowa bandana", "[A] Zolto-brazowa bandana", 0, 2, 50);
	InitProduct(ORDER_GANG, ITEM_TYPE_ACCESSORY, 18897, "B³êkitna bandana", "[A] Blekitna bandana", 0, 2, 50);
	InitProduct(ORDER_GANG, ITEM_TYPE_ACCESSORY, 18898, "Zielona bandana", "[A] Zielona bandana", 0, 2, 50);
	
	// LSPD
	InitProduct(ORDER_LSPD, ITEM_TYPE_WEAPON, 348, "Desert Eagle", "", 24, 40, 600);
	InitProduct(ORDER_LSPD, ITEM_TYPE_WEAPON, 349, "Shotgun", "", 25, 50, 1200);
	InitProduct(ORDER_LSPD, ITEM_TYPE_WEAPON, 353, "MP5", "", 29, 400, 2000);
	InitProduct(ORDER_LSPD, ITEM_TYPE_WEAPON, 356, "M4", "", 31, 400, 3500);
	InitProduct(ORDER_LSPD, ITEM_TYPE_WEAPON, 334, "Pa³ka", "", 3, 1, 20);
	InitProduct(ORDER_LSPD, ITEM_TYPE_MEGAFON, 19320, "Megafon", "Megafon", 0, 0, 50);
	InitProduct(ORDER_LSPD, ITEM_TYPE_AMMO, 3052, "Amunicja krótka", "", 2, 100, 50);
	InitProduct(ORDER_LSPD, ITEM_TYPE_AMMO, 3052, "Amunicja d³uga", "", 3, 300, 150);
	InitProduct(ORDER_LSPD, ITEM_TYPE_AMMO, 3052, "Amunicja do granatów", "", 4, 20, 300);
	InitProduct(ORDER_LSPD, ITEM_TYPE_AMMO, 3052, "Amunicja do broni specjalnych", "", 5, 5, 600);
	InitProduct(ORDER_LSPD, ITEM_TYPE_AMMO, 3052, "Amunicja do snajperek", "", 6, 20, 499);
	InitProduct(ORDER_LSPD, ITEM_TYPE_ACCESSORY, 18636, "Policyjna baseballówka", "[A] Policyjna baseballowka", 0, 2, 20);
	InitProduct(ORDER_LSPD, ITEM_TYPE_ACCESSORY, 19099, "Kapelusz szeryfa", "[A] Kapelusz szeryfa", 0, 2, 30);
	InitProduct(ORDER_LSPD, ITEM_TYPE_ACCESSORY, 19100, "Br¹zowy kapelusz szeryfa", "[A] Brazowy kapelusz szeryfa", 0, 2, 30);
	InitProduct(ORDER_LSPD, ITEM_TYPE_ACCESSORY, 18637, "Tarcza SWAT", "[A] Tarcza SWAT", 0, 5, 50);
	InitProduct(ORDER_LSPD, ITEM_TYPE_ACCESSORY, 18641, "Latarka", "[A] Latarka", 0, 5, 30);
	
	// LSN
	InitProduct(ORDER_LSN, ITEM_TYPE_MEGAFON, 19320, "Megafon", "Megafon", 0, 0, 50);
	
	// Syndykat
	InitProduct(ORDER_SYND, ITEM_TYPE_WEAPON, 346, "9mm", "", 22, 80, 100);
	InitProduct(ORDER_SYND, ITEM_TYPE_WEAPON, 347, "9mm z t³umikiem", "", 23, 80, 300);
	InitProduct(ORDER_SYND, ITEM_TYPE_WEAPON, 348, "Desert Eagle", "", 24, 40, 600);
	
	InitProduct(ORDER_SYND, ITEM_TYPE_WEAPON, 372, "Tec", "", 32, 100, 600);
	InitProduct(ORDER_SYND, ITEM_TYPE_WEAPON, 352, "Uzi", "", 28, 100, 600);
	InitProduct(ORDER_SYND, ITEM_TYPE_WEAPON, 353, "MP5", "", 29, 100, 800);
	
	InitProduct(ORDER_SYND, ITEM_TYPE_WEAPON, 350, "Obrzyn", "", 26, 100, 400);
	InitProduct(ORDER_SYND, ITEM_TYPE_WEAPON, 349, "Shotgun", "", 25, 100, 500);
	InitProduct(ORDER_SYND, ITEM_TYPE_WEAPON, 351, "Spas", "", 27, 100, 2000);
	
	InitProduct(ORDER_SYND, ITEM_TYPE_WEAPON, 357, "Rifle", "", 33, 100, 900);
	InitProduct(ORDER_SYND, ITEM_TYPE_WEAPON, 358, "Snajperka", "", 34, 100, 1500);
	
	InitProduct(ORDER_SYND, ITEM_TYPE_WEAPON, 355, "AK47", "", 30, 100, 2500);
	InitProduct(ORDER_SYND, ITEM_TYPE_WEAPON, 356, "M4", "", 31, 100, 3000);
	
	InitProduct(ORDER_SYND, ITEM_TYPE_WEAPON, 359, "RPG", "", 35, 5, 15000);
	
	InitProduct(ORDER_SYND, ITEM_TYPE_AMMO, 3052, "Amunicja krótka", "", 2, 100, 50);
	InitProduct(ORDER_SYND, ITEM_TYPE_AMMO, 3052, "Amunicja d³uga", "", 3, 300, 150);
	InitProduct(ORDER_SYND, ITEM_TYPE_AMMO, 3052, "Amunicja do granatów", "", 4, 20, 300);
	InitProduct(ORDER_SYND, ITEM_TYPE_AMMO, 3052, "Amunicja do broni specjalnych", "", 5, 5, 600);
	InitProduct(ORDER_SYND, ITEM_TYPE_AMMO, 3052, "Amunicja do snajperek", "", 6, 20, 499);
	
	// FBI
	InitProduct(ORDER_FBI, ITEM_TYPE_WEAPON, 346, "9mm", "", 22, 80, 300);
	InitProduct(ORDER_FBI, ITEM_TYPE_WEAPON, 347, "9mm z t³umikiem", "", 23, 80, 400);
	InitProduct(ORDER_FBI, ITEM_TYPE_WEAPON, 348, "Desert Eagle", "", 24, 40, 400);
	InitProduct(ORDER_FBI, ITEM_TYPE_WEAPON, 356, "M4", "", 31, 100, 800);
	InitProduct(ORDER_FBI, ITEM_TYPE_WEAPON, 351, "Spas", "", 27, 100, 1000);
	InitProduct(ORDER_FBI, ITEM_TYPE_WEAPON, 349, "Shotgun", "", 25, 100, 800);
	InitProduct(ORDER_FBI, ITEM_TYPE_WEAPON, 358, "Snajperka", "", 34, 100, 2000);
	InitProduct(ORDER_FBI, ITEM_TYPE_WEAPON, 355, "AK47", "", 30, 100, 800);
	InitProduct(ORDER_FBI, ITEM_TYPE_WEAPON, 367, "Aparat", "", 43, 100000, 100);
	InitProduct(ORDER_FBI, ITEM_TYPE_MASK, 19036, "Kominiarka", "", -1, 0, 500);	
	
	InitProduct(ORDER_FBI, ITEM_TYPE_AMMO, 3052, "Amunicja krótka", "", 2, 100, 50);
	InitProduct(ORDER_FBI, ITEM_TYPE_AMMO, 3052, "Amunicja d³uga", "", 3, 300, 150);
	InitProduct(ORDER_FBI, ITEM_TYPE_AMMO, 3052, "Amunicja do granatów", "", 4, 20, 300);
	InitProduct(ORDER_FBI, ITEM_TYPE_AMMO, 3052, "Amunicja do broni specjalnych", "", 5, 5, 600);
	InitProduct(ORDER_FBI, ITEM_TYPE_AMMO, 3052, "Amunicja do snajperek", "", 6, 20, 499);
	
	InitProduct(ORDER_FBI, ITEM_TYPE_ACCESSORY, 18636, "Policyjna baseballówka", "[A] Policyjna baseballowka", 0, 2, 20);
	InitProduct(ORDER_FBI, ITEM_TYPE_ACCESSORY, 19099, "Kapelusz szeryfa", "[A] Kapelusz szeryfa", 0, 2, 30);
	InitProduct(ORDER_FBI, ITEM_TYPE_ACCESSORY, 19100, "Br¹zowy kapelusz szeryfa", "[A] Brazowy kapelusz szeryfa", 0, 2, 30);
	InitProduct(ORDER_FBI, ITEM_TYPE_ACCESSORY, 18637, "Tarcza SWAT", "[A] Tarcza SWAT", 0, 5, 50);
	InitProduct(ORDER_FBI, ITEM_TYPE_ACCESSORY, 18641, "Latarka", "[A] Latarka", 0, 5, 30);
	
	// GYM
	InitProduct(ORDER_GYM, ITEM_TYPE_GYM_PASS, 2663, "Karnet (10 minut)", "", 0, 10, 50);
}

stock InitProduct(category, type, model, listname[], name[], value1, value2, price)
{
	new id = Iter_Free(Products);
	
	Product[id][product_category] = category;
	Product[id][product_type] = type;
	Product[id][product_model] = model;
	Product[id][product_value1] = value1;
	Product[id][product_value2] = value2;
	Product[id][product_price] = price;
	
	strcopy(Product[id][product_list_name], listname, 40);
	strcopy(Product[id][product_name], name, 40);
	
	Iter_Add(Products, id);
}

stock GetPackageDoorByUid(p_uid)
{
	new Cache:result, d_uid = -1;
	result = mysql_query(g_sql, sprintf("SELECT package_dooruid FROM `crp_packages` WHERE `package_uid` = %d", p_uid));
	
	if( cache_get_rows() > 0 ) {
		d_uid = cache_get_int(0, "package_dooruid");
	}
	
	cache_delete(result);
	
	return d_uid;	
}