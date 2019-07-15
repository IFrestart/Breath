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
		user_parent_id int unsigned not null,
		user_index int unsigned not null primary key auto_increment,
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

create trigger insert_corp after insert on corp for each row  insert into users(user_parent_id,user_type,user_id,user_corp_id)values(1,4,new.corp_name,new.corp_id);

insert into corp (corp_name)values('system');

insert into users(user_id,user_type,user_pwd,user_corp_id) select "admin",65536,aes_encrypt('123456','users.user_pwd'),corp_id from corp where corp_id=(select max(corp_id) from corp);

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



