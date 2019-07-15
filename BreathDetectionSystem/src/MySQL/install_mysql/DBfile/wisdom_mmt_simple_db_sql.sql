use mmt_simple_db;

create table corp (
		corp_id int unsigned not null primary key auto_increment,
		corp_name varbinary(64) not null,
		corp_addr varbinary(64) not null,
		corp_tel varbinary(20) not null,
		corp_fax varbinary(20) not null,
		corp_mail varbinary(64) not null,
		register_time timestamp not null,
		meeting_num int unsigned  not null,
		corp_desc varbinary(64) not null)
		engine=innodb auto_increment=100000 default character set utf8;

create table users (
		user_index int unsigned not null primary key auto_increment,
		user_parent_id int unsigned not null ,
		user_id varbinary(129) not null,
		user_nick varbinary(129) not null,
		user_type int unsigned not null,
		user_corp_id int unsigned not null,
		user_pwd varbinary(32) not null,
		register_time timestamp not null,
		INDEX user_id_index(user_index),
		unique index user_name(user_id),
		foreign key(user_corp_id) references corp(corp_id) on delete cascade on update cascade)
		engine=innodb auto_increment=2 default character set utf8;

create table groupInfo(
		group_id int unsigned not null primary key,
		group_status int unsigned not null,
		group_flag int unsigned not null,
		group_maxnum smallint unsigned not null,
		group_funtype int unsigned not null,
		group_pwd varbinary(16) not null,
		group_hostpwd varbinary(16) not null,
		group_name varbinary(64) not null,
		group_corpid int unsigned not null,
		group_meetingtype smallint unsigned not null,
		group_desc varbinary(255) not null,
		group_adminid varbinary(129) not null,
		group_datalen smallint unsigned not null,
		group_date tinyblob,
		unique index group_nickname(group_name),
		foreign key(group_corpid) references corp(corp_id) on delete cascade on update cascade) 
		engine=innodb default character set utf8;

create table Usergrant (
		grant_index int unsigned not null primary key auto_increment,
		grant_name varbinary(129) not null,
		grant_nick varbinary(129) not null,
		grant_parent_index int unsigned not null,
		grant_type int unsigned not null,
		grant_corp_id int unsigned not null,
		rootID bigint unsigned not null,
		mes_count int unsigned not null,
		unique key(grant_name,grant_parent_index,grant_type),
		INDEX parent_index(grant_parent_index),
		index corp_id(grant_corp_id),
		foreign key(grant_corp_id) references corp(corp_id) on delete cascade on update cascade)
		engine=innodb auto_increment=2 default character set utf8;
		

create table phoneInfo(
		phone_id varbinary(129)not null,
		phone_nick varbinary(129) not null,
		phone_flag int unsigned not null,
		phone_num int unsigned not null,
		phone_starttime TIME,
		phone_limittime TIME,
		phone_remark varbinary(255) not null,
		phone_corp_id int unsigned not null,
		primary key (phone_id),
		index phone_start_time(phone_starttime),
		index phone_stop_time(phone_limittime),
		foreign key(phone_corp_id) references corp(corp_id) on delete cascade on update cascade)
		engine=innodb default character set utf8;

create table Massage(
		phone_id varbinary(129) not null,
		alarm_time  Time,
		alarm_date  date,
		massage tinyblob not null,
		alarm_type smallint unsigned,
		messagelen smallint unsigned,
		index phone_name(phone_id),
		foreign key(phone_id) references phoneInfo(phone_id) on delete cascade on update cascade)
		engine=innodb default character set utf8;


create trigger insert_user after insert on users for each row  insert into Usergrant(grant_parent_index,grant_type,grant_name,grant_nick,grant_corp_id)values(0,new.user_type,new.user_id,new.user_nick,new.user_corp_id);

create trigger update_user after update on users for each row update Usergrant set grant_nick=new.user_nick where grant_name=old.user_id and grant_corp_id=new.user_corp_id ;

create trigger insert_phoneInfo after insert on phoneInfo for each row insert into Usergrant(grant_parent_index,grant_type,grant_name,grant_nick,grant_corp_id)values(0,7,new.phone_id,new.phone_nick,new.phone_corp_id);

create trigger update_phoneInfo after update on phoneInfo for each row update Usergrant set grant_nick=new.phone_nick where grant_name=old.phone_id and grant_type=7;

insert into corp (corp_name)values('system');

insert into users(user_id,user_type,user_pwd,user_corp_id) select "admin",65536,aes_encrypt('123456','users.user_pwd'),corp_id from corp where corp_id=(select max(corp_id) from corp);
