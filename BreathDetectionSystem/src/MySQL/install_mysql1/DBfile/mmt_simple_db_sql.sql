use mmt_simple_db;

create table Users (
		user_index int unsigned not null primary key auto_increment,
		user_name varbinary(129) not null,
		user_nick varbinary(129) not null,
		user_pwd varbinary(17) not null,
		user_type int unsigned not null,
		register_time timestamp not null,
		unique index idx_name(user_name),
		engine=innodb auto_increment=2 default character set utf8;

create table UserGrant (
		grant_index int unsigned not null primary key auto_increment,
		grant_name varbinary(129) not null,
		grant_parent_index bigint unsigned not null,
		grant_type bigint unsigned not null,
		user_idx int unsigned not null,
		mes_count int unsigned not null,
		unique key(grant_name, grant_parent_index,grant_type, user_idx),
		index idx_parent(grant_parent_index),
		index idx_root(user_idx),
		foreign key(user_idx) references Users(user_index) on delete cascade on update cascade)
		engine=innodb auto_increment=2 default character set utf8;


create trigger insert_user after insert on Users for each row  insert into UserGrant(grant_parent_index,grant_type,grant_name,user_idx)values(0,1,new.user_nick,new.user_index);

create trigger update_user after update on Users for each row update UserGrant set grant_name=new.user_nick where grant_parent_index = 0 and user_idx=old.user_index;

insert into Users (user_name,user_nick,user_pwd,user_type) values ('admin','admin',aes_encrypt('123456','bingetout'),65536);
insert into Users (user_name,user_nick,user_pwd,user_type) values ('wangm','wangm',aes_encrypt('123456','bingetout'),1);
insert into UserGrant (grant_name, grant_parent_index, grant_type, user_idx) select 'my1',3,1,user_idx from UserGrant where grant_name="wangm";
insert into UserGrant (grant_name, grant_parent_index, grant_type, user_idx) select 'my2',4,1,user_idx from UserGrant where grant_name="wangm";
insert into UserGrant (grant_name, grant_parent_index, grant_type, user_idx) select 'my3',4,2,user_idx from UserGrant where grant_name="wangm";
insert into UserGrant (grant_name, grant_parent_index, grant_type, user_idx) select 'my5',5,1,user_idx from UserGrant where grant_name="wangm"

insert into Users (user_name,user_nick,user_pwd,user_type) values ('wangm','wangm',aes_encrypt('123456','bingetout'),1);
insert into Users (user_name,user_nick,user_pwd,user_type) values ('zhugf','zhugf',aes_encrypt('123456','bingetout'),1);
insert into Users (user_name,user_nick,user_pwd,user_type) values ('zhoujj','zhoujj',aes_encrypt('123456','bingetout'),1);
insert into Users (user_name,user_nick,user_pwd,user_type) values ('liuchen','liuchen',aes_encrypt('123456','bingetout'),1);
insert into Users (user_name,user_nick,user_pwd,user_type) values ('renfz','renfz',aes_encrypt('123456','bingetout'),1);

