from pathlib import Path
ROOT=Path('/mnt/data/ELIT21_Shopify_AliExpress_Autonomous_Manager-v6.0.0-SHOPIFY-TRANSACTIONAL')
p=ROOT/'src/elit21/storage/Database.cpp'
t=p.read_text(encoding='utf-8')

def rep(old,new,count=1):
 global t
 if old not in t: raise RuntimeError('missing '+old[:100])
 t=t.replace(old,new,count)

rep('#include "elit21/security/Crypto.h"\n', '#include "elit21/security/Crypto.h"\n#include "elit21/shopify/ShopifyOrderStateMachine.h"\n')
rep('''    const char* sql =
        "INSERT OR IGNORE INTO orders(shopify_order_id,aliexpress_order_id,total_cad,status,payload_json,"
        "idempotency_key,created_at,updated_at) VALUES(?,?,?,?,?,?,?,?)";
''','''    const char* sql =
        "INSERT OR IGNORE INTO orders(shopify_order_id,aliexpress_order_id,customer_email,total_cad,currency,"
        "status,payload_json,idempotency_key,created_at,updated_at) VALUES(?,?,?,?,?,?,?,?,?,?)";
''')
rep('''    sqlite3_bind_text(statement, 1, order.shopify_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 2, order.ae_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(statement, 3, order.total);
    sqlite3_bind_text(statement, 4, order.status.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 5, order.payload.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 6, order.idempotency_key.c_str(), -1, SQLITE_TRANSIENT);
    const auto now = util::utcNowIso();
    sqlite3_bind_text(statement, 7, now.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 8, now.c_str(), -1, SQLITE_TRANSIENT);
''','''    sqlite3_bind_text(statement, 1, order.shopify_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 2, order.ae_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 3, order.customer_email.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(statement, 4, order.total);
    sqlite3_bind_text(statement, 5, order.currency.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 6, order.status.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 7, order.payload.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 8, order.idempotency_key.c_str(), -1, SQLITE_TRANSIENT);
    const auto now = util::utcNowIso();
    sqlite3_bind_text(statement, 9, now.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 10, now.c_str(), -1, SQLITE_TRANSIENT);
''')

insert_before='Result<std::vector<StoredOrder>> Database::ordersByStatus(const std::string& status, int limit) {'
if insert_before not in t: raise RuntimeError('ordersByStatus missing')
methods=r'''
Result<bool> Database::saveOrderWithLines(const StoredOrder& order,
                                          const std::vector<StoredOrderLine>& lines) {
    if (order.shopify_id.empty() || order.status.empty() || order.idempotency_key.empty())
        return Result<bool>::failure("Shopify order identity, status and idempotency key are required");
    if (lines.empty()) return Result<bool>::failure("Shopify order must contain at least one line");
    for (const auto& line : lines) {
        if (line.quantity <= 0 || line.unit_price_cad < 0.0)
            return Result<bool>::failure("Shopify order line quantity and price are invalid");
        if (line.aliexpress_product_id.empty() || line.aliexpress_sku_id.empty())
            return Result<bool>::failure("Shopify order line has no AliExpress mapping");
    }
    std::lock_guard lock(mutex_);
    if (!db_) return Result<bool>::failure("Database is not open");
    auto begin = executeUnlocked("BEGIN IMMEDIATE;");
    if (!begin) return Result<bool>::failure(begin.error());
    sqlite3_stmt* order_statement = nullptr;
    const char* order_sql =
        "INSERT OR IGNORE INTO orders(shopify_order_id,aliexpress_order_id,customer_email,total_cad,currency,"
        "status,payload_json,idempotency_key,created_at,updated_at) VALUES(?,?,?,?,?,?,?,?,?,?)";
    if (sqlite3_prepare_v2(db_, order_sql, -1, &order_statement, nullptr) != SQLITE_OK) {
        executeUnlocked("ROLLBACK;"); return Result<bool>::failure(sqlite3_errmsg(db_));
    }
    const auto now = util::utcNowIso();
    sqlite3_bind_text(order_statement,1,order.shopify_id.c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_text(order_statement,2,order.ae_id.c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_text(order_statement,3,order.customer_email.c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_double(order_statement,4,order.total);
    sqlite3_bind_text(order_statement,5,order.currency.c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_text(order_statement,6,order.status.c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_text(order_statement,7,order.payload.c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_text(order_statement,8,order.idempotency_key.c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_text(order_statement,9,now.c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_text(order_statement,10,now.c_str(),-1,SQLITE_TRANSIENT);
    const int order_code=sqlite3_step(order_statement); const bool inserted=sqlite3_changes(db_)>0;
    const std::string order_error=order_code==SQLITE_DONE?std::string{}:std::string(sqlite3_errmsg(db_));
    sqlite3_finalize(order_statement);
    if(order_code!=SQLITE_DONE){executeUnlocked("ROLLBACK;");return Result<bool>::failure(order_error);}
    if(!inserted){auto c=executeUnlocked("COMMIT;");return c?Result<bool>::success(false):Result<bool>::failure(c.error());}
    const auto order_id=sqlite3_last_insert_rowid(db_);
    sqlite3_stmt* line_statement=nullptr;
    const char* line_sql="INSERT INTO order_lines(order_id,shopify_line_item_id,shopify_variant_id,aliexpress_product_id,aliexpress_sku_id,sku,title,quantity,unit_price_cad,status) VALUES(?,?,?,?,?,?,?,?,?,?)";
    if(sqlite3_prepare_v2(db_,line_sql,-1,&line_statement,nullptr)!=SQLITE_OK){executeUnlocked("ROLLBACK;");return Result<bool>::failure(sqlite3_errmsg(db_));}
    for(const auto& line:lines){sqlite3_reset(line_statement);sqlite3_clear_bindings(line_statement);sqlite3_bind_int64(line_statement,1,order_id);sqlite3_bind_text(line_statement,2,line.shopify_line_id.c_str(),-1,SQLITE_TRANSIENT);sqlite3_bind_text(line_statement,3,line.shopify_variant_id.c_str(),-1,SQLITE_TRANSIENT);sqlite3_bind_text(line_statement,4,line.aliexpress_product_id.c_str(),-1,SQLITE_TRANSIENT);sqlite3_bind_text(line_statement,5,line.aliexpress_sku_id.c_str(),-1,SQLITE_TRANSIENT);sqlite3_bind_text(line_statement,6,line.sku.c_str(),-1,SQLITE_TRANSIENT);sqlite3_bind_text(line_statement,7,line.title.c_str(),-1,SQLITE_TRANSIENT);sqlite3_bind_int(line_statement,8,line.quantity);sqlite3_bind_double(line_statement,9,line.unit_price_cad);sqlite3_bind_text(line_statement,10,line.status.c_str(),-1,SQLITE_TRANSIENT);if(sqlite3_step(line_statement)!=SQLITE_DONE){auto e=std::string(sqlite3_errmsg(db_));sqlite3_finalize(line_statement);executeUnlocked("ROLLBACK;");return Result<bool>::failure(e);}}
    sqlite3_finalize(line_statement);
    const std::string event="INSERT INTO shopify_order_events(order_id,event_type,from_status,to_status,context_json,created_at) VALUES("+std::to_string(order_id)+",'order_received','',"+sqlQuote(order.status)+","+sqlQuote(order.payload)+","+sqlQuote(now)+");";
    auto recorded=executeUnlocked(event);if(!recorded){executeUnlocked("ROLLBACK;");return Result<bool>::failure(recorded.error());}
    auto commit=executeUnlocked("COMMIT;");return commit?Result<bool>::success(true):Result<bool>::failure(commit.error());
}

Result<std::vector<StoredOrderLine>> Database::orderLines(const std::string& shopify_order_id) {
    std::lock_guard lock(mutex_); if(!db_)return Result<std::vector<StoredOrderLine>>::failure("Database is not open");
    sqlite3_stmt* statement=nullptr;const char* sql="SELECT l.id,l.order_id,COALESCE(l.shopify_line_item_id,''),COALESCE(l.shopify_variant_id,''),l.aliexpress_product_id,l.aliexpress_sku_id,COALESCE(l.sku,''),COALESCE(l.title,''),l.quantity,l.unit_price_cad,l.status FROM order_lines l JOIN orders o ON o.id=l.order_id WHERE o.shopify_order_id=? ORDER BY l.id";
    if(sqlite3_prepare_v2(db_,sql,-1,&statement,nullptr)!=SQLITE_OK)return Result<std::vector<StoredOrderLine>>::failure(sqlite3_errmsg(db_));
    sqlite3_bind_text(statement,1,shopify_order_id.c_str(),-1,SQLITE_TRANSIENT);std::vector<StoredOrderLine> output;
    while(sqlite3_step(statement)==SQLITE_ROW){StoredOrderLine line;line.id=sqlite3_column_int64(statement,0);line.order_id=sqlite3_column_int64(statement,1);line.shopify_line_id=columnText(statement,2);line.shopify_variant_id=columnText(statement,3);line.aliexpress_product_id=columnText(statement,4);line.aliexpress_sku_id=columnText(statement,5);line.sku=columnText(statement,6);line.title=columnText(statement,7);line.quantity=sqlite3_column_int(statement,8);line.unit_price_cad=sqlite3_column_double(statement,9);line.status=columnText(statement,10);output.push_back(std::move(line));}
    sqlite3_finalize(statement);return Result<std::vector<StoredOrderLine>>::success(std::move(output));
}

'''
t=t.replace(insert_before, methods+insert_before,1)
# OrdersByStatus query
rep('''            "SELECT id,shopify_order_id,COALESCE(aliexpress_order_id,''),status,payload_json,"
            "idempotency_key,total_cad FROM orders WHERE status=? ORDER BY id LIMIT ?",
''','''            "SELECT id,shopify_order_id,COALESCE(aliexpress_order_id,''),status,payload_json,"
            "idempotency_key,total_cad,COALESCE(customer_email,''),currency FROM orders WHERE status=? ORDER BY id LIMIT ?",
''')
rep('''        order.total = sqlite3_column_double(statement, 6);
        output.push_back(std::move(order));
''','''        order.total = sqlite3_column_double(statement, 6);
        order.customer_email = columnText(statement, 7);
        order.currency = columnText(statement, 8);
        output.push_back(std::move(order));
''')
# Insert order lookup/redaction/transition before updateOrderSupplierId
needle='Result<void> Database::updateOrderSupplierId(const std::string& shopify_id,'
if needle not in t: raise RuntimeError('updateOrderSupplierId missing')
extra=r'''
Result<std::vector<StoredOrder>> Database::ordersByShopifyIds(const std::vector<std::string>& ids){if(ids.empty())return Result<std::vector<StoredOrder>>::success({});std::lock_guard lock(mutex_);if(!db_)return Result<std::vector<StoredOrder>>::failure("Database is not open");std::string marks;for(std::size_t i=0;i<ids.size();++i){if(i)marks+=',';marks+='?';}const std::string sql="SELECT id,shopify_order_id,COALESCE(aliexpress_order_id,''),status,payload_json,idempotency_key,total_cad,COALESCE(customer_email,''),currency FROM orders WHERE shopify_order_id IN("+marks+") ORDER BY id";sqlite3_stmt*s=nullptr;if(sqlite3_prepare_v2(db_,sql.c_str(),-1,&s,nullptr)!=SQLITE_OK)return Result<std::vector<StoredOrder>>::failure(sqlite3_errmsg(db_));for(std::size_t i=0;i<ids.size();++i)sqlite3_bind_text(s,int(i+1),ids[i].c_str(),-1,SQLITE_TRANSIENT);std::vector<StoredOrder>o;while(sqlite3_step(s)==SQLITE_ROW){StoredOrder x;x.id=sqlite3_column_int64(s,0);x.shopify_id=columnText(s,1);x.ae_id=columnText(s,2);x.status=columnText(s,3);x.payload=columnText(s,4);x.idempotency_key=columnText(s,5);x.total=sqlite3_column_double(s,6);x.customer_email=columnText(s,7);x.currency=columnText(s,8);o.push_back(std::move(x));}sqlite3_finalize(s);return Result<std::vector<StoredOrder>>::success(std::move(o));}
Result<int> Database::redactShopifyOrders(const std::vector<std::string>& ids,const std::string& reason){if(ids.empty())return Result<int>::success(0);std::lock_guard lock(mutex_);if(!db_)return Result<int>::failure("Database is not open");auto b=executeUnlocked("BEGIN IMMEDIATE;");if(!b)return Result<int>::failure(b.error());std::string marks;for(std::size_t i=0;i<ids.size();++i){if(i)marks+=',';marks+='?';}const std::string sql="UPDATE orders SET customer_email=NULL,payload_json=?,last_error=?,updated_at=? WHERE shopify_order_id IN("+marks+")";sqlite3_stmt*s=nullptr;if(sqlite3_prepare_v2(db_,sql.c_str(),-1,&s,nullptr)!=SQLITE_OK){executeUnlocked("ROLLBACK;");return Result<int>::failure(sqlite3_errmsg(db_));}Json j=Json::object();j.set("redacted",true);j.set("reason",reason);j.set("redacted_at",util::utcNowIso());auto now=util::utcNowIso(),body=j.dump();sqlite3_bind_text(s,1,body.c_str(),-1,SQLITE_TRANSIENT);sqlite3_bind_text(s,2,reason.c_str(),-1,SQLITE_TRANSIENT);sqlite3_bind_text(s,3,now.c_str(),-1,SQLITE_TRANSIENT);for(std::size_t i=0;i<ids.size();++i)sqlite3_bind_text(s,int(i+4),ids[i].c_str(),-1,SQLITE_TRANSIENT);int code=sqlite3_step(s),changed=sqlite3_changes(db_);auto err=code==SQLITE_DONE?std::string{}:std::string(sqlite3_errmsg(db_));sqlite3_finalize(s);if(code!=SQLITE_DONE){executeUnlocked("ROLLBACK;");return Result<int>::failure(err);}auto c=executeUnlocked("COMMIT;");return c?Result<int>::success(changed):Result<int>::failure(c.error());}
Result<int> Database::redactAllShopifyCustomerData(const std::string& reason){std::lock_guard lock(mutex_);if(!db_)return Result<int>::failure("Database is not open");auto b=executeUnlocked("BEGIN IMMEDIATE;");if(!b)return Result<int>::failure(b.error());Json j=Json::object();j.set("redacted",true);j.set("reason",reason);j.set("redacted_at",util::utcNowIso());auto r=executeUnlocked("UPDATE orders SET customer_email=NULL,payload_json="+sqlQuote(j.dump())+",last_error="+sqlQuote(reason)+",updated_at="+sqlQuote(util::utcNowIso())+";");if(!r){executeUnlocked("ROLLBACK;");return Result<int>::failure(r.error());}int changed=sqlite3_changes(db_);auto m=executeUnlocked("DELETE FROM customer_messages;");if(!m){executeUnlocked("ROLLBACK;");return Result<int>::failure(m.error());}auto c=executeUnlocked("COMMIT;");return c?Result<int>::success(changed):Result<int>::failure(c.error());}
Result<bool> Database::transitionOrderStatus(const std::string&id,const std::vector<std::string>&expected,const std::string&next,const std::string&event,const std::string&context,const std::string&last_error){if(id.empty()||expected.empty()||next.empty()||event.empty())return Result<bool>::failure("Order transition parameters are incomplete");for(const auto&s:expected){auto v=shopify::ShopifyOrderStateMachine::validateTransition(s,next);if(!v)return Result<bool>::failure(v.error());}std::lock_guard lock(mutex_);if(!db_)return Result<bool>::failure("Database is not open");auto b=executeUnlocked("BEGIN IMMEDIATE;");if(!b)return Result<bool>::failure(b.error());std::string states;for(std::size_t i=0;i<expected.size();++i){if(i)states+=',';states+=sqlQuote(expected[i]);}auto now=util::utcNowIso();auto u=executeUnlocked("UPDATE orders SET status="+sqlQuote(next)+",last_error="+sqlQuote(last_error)+",updated_at="+sqlQuote(now)+" WHERE shopify_order_id="+sqlQuote(id)+" AND status IN("+states+");");if(!u){executeUnlocked("ROLLBACK;");return Result<bool>::failure(u.error());}if(sqlite3_changes(db_)==0){auto c=executeUnlocked("COMMIT;");return c?Result<bool>::success(false):Result<bool>::failure(c.error());}auto e=executeUnlocked("INSERT INTO shopify_order_events(order_id,event_type,from_status,to_status,context_json,created_at) SELECT id,"+sqlQuote(event)+","+sqlQuote(expected.front())+","+sqlQuote(next)+","+sqlQuote(context)+","+sqlQuote(now)+" FROM orders WHERE shopify_order_id="+sqlQuote(id)+";");if(!e){executeUnlocked("ROLLBACK;");return Result<bool>::failure(e.error());}auto c=executeUnlocked("COMMIT;");return c?Result<bool>::success(true):Result<bool>::failure(c.error());}

'''
t=t.replace(needle,extra+needle,1)
# Insert leases/privacy before setRuntimeState
needle2='Result<void> Database::setRuntimeState(const std::string& key, const std::string& value_json) {'
if needle2 not in t: raise RuntimeError('setRuntimeState missing')
extra2=r'''
Result<bool> Database::acquireLease(const std::string&name,const std::string&owner,int ttl){if(name.empty()||owner.empty())return Result<bool>::failure("Lease name and owner are required");std::lock_guard lock(mutex_);if(!db_)return Result<bool>::failure("Database is not open");sqlite3_stmt*s=nullptr;const char*sql="INSERT INTO shopify_sync_leases(lease_name,owner_id,acquired_at,expires_at) VALUES(?,?,CURRENT_TIMESTAMP,datetime('now',?)) ON CONFLICT(lease_name) DO UPDATE SET owner_id=excluded.owner_id,acquired_at=CURRENT_TIMESTAMP,expires_at=excluded.expires_at WHERE shopify_sync_leases.owner_id=excluded.owner_id OR datetime(shopify_sync_leases.expires_at)<=datetime('now')";if(sqlite3_prepare_v2(db_,sql,-1,&s,nullptr)!=SQLITE_OK)return Result<bool>::failure(sqlite3_errmsg(db_));auto interval="+"+std::to_string(std::max(1,ttl))+" seconds";sqlite3_bind_text(s,1,name.c_str(),-1,SQLITE_TRANSIENT);sqlite3_bind_text(s,2,owner.c_str(),-1,SQLITE_TRANSIENT);sqlite3_bind_text(s,3,interval.c_str(),-1,SQLITE_TRANSIENT);int code=sqlite3_step(s);bool acquired=sqlite3_changes(db_)>0;auto err=code==SQLITE_DONE?std::string{}:std::string(sqlite3_errmsg(db_));sqlite3_finalize(s);return code==SQLITE_DONE?Result<bool>::success(acquired):Result<bool>::failure(err);}
Result<void> Database::releaseLease(const std::string&name,const std::string&owner){return prepareAndStep("DELETE FROM shopify_sync_leases WHERE lease_name=? AND owner_id=?",[&](sqlite3_stmt*s){sqlite3_bind_text(s,1,name.c_str(),-1,SQLITE_TRANSIENT);sqlite3_bind_text(s,2,owner.c_str(),-1,SQLITE_TRANSIENT);});}
Result<void> Database::recordShopifyPrivacyRequest(const ShopifyPrivacyRequestRecord&r){if(r.request_id.empty()||r.topic.empty()||r.shop_domain.empty()||r.payload_hash.empty())return Result<void>::failure("Shopify privacy request identity is incomplete");std::lock_guard lock(mutex_);if(!db_)return Result<void>::failure("Database is not open");auto b=executeUnlocked("BEGIN IMMEDIATE;");if(!b)return b;sqlite3_stmt*s=nullptr;const char*sql="INSERT OR IGNORE INTO shopify_privacy_requests(request_id,topic,shop_domain,customer_id,order_ids_json,payload_hash,status,received_at,updated_at) VALUES(?,?,?,?,?,?,?,CURRENT_TIMESTAMP,CURRENT_TIMESTAMP)";if(sqlite3_prepare_v2(db_,sql,-1,&s,nullptr)!=SQLITE_OK){executeUnlocked("ROLLBACK;");return Result<void>::failure(sqlite3_errmsg(db_));}sqlite3_bind_text(s,1,r.request_id.c_str(),-1,SQLITE_TRANSIENT);sqlite3_bind_text(s,2,r.topic.c_str(),-1,SQLITE_TRANSIENT);sqlite3_bind_text(s,3,r.shop_domain.c_str(),-1,SQLITE_TRANSIENT);sqlite3_bind_text(s,4,r.customer_id.c_str(),-1,SQLITE_TRANSIENT);sqlite3_bind_text(s,5,r.orders_json.c_str(),-1,SQLITE_TRANSIENT);sqlite3_bind_text(s,6,r.payload_hash.c_str(),-1,SQLITE_TRANSIENT);sqlite3_bind_text(s,7,r.status.c_str(),-1,SQLITE_TRANSIENT);int code=sqlite3_step(s);bool inserted=sqlite3_changes(db_)>0;auto err=code==SQLITE_DONE?std::string{}:std::string(sqlite3_errmsg(db_));sqlite3_finalize(s);if(code!=SQLITE_DONE){executeUnlocked("ROLLBACK;");return Result<void>::failure(err);}if(!inserted){sqlite3_stmt*q=nullptr;if(sqlite3_prepare_v2(db_,"SELECT payload_hash FROM shopify_privacy_requests WHERE request_id=?",-1,&q,nullptr)!=SQLITE_OK){executeUnlocked("ROLLBACK;");return Result<void>::failure(sqlite3_errmsg(db_));}sqlite3_bind_text(q,1,r.request_id.c_str(),-1,SQLITE_TRANSIENT);std::string hash;if(sqlite3_step(q)==SQLITE_ROW)hash=columnText(q,0);sqlite3_finalize(q);if(hash!=r.payload_hash){executeUnlocked("ROLLBACK;");return Result<void>::failure("Shopify privacy request replay payload mismatch");}}return executeUnlocked("COMMIT;");}

'''
t=t.replace(needle2,extra2+needle2,1)
p.write_text(t,encoding='utf-8')
print('phase2 database patched')
